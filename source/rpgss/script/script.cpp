/*
    The MIT License (MIT)

    Copyright (c) 2014 Anatoli Steinmark

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include "../debug/debug.hpp"
#include "../common/stringutil.hpp"
#include "../core/ByteArray.hpp"
#include "../io/io.hpp"
#include "script.hpp"

#define RPGSS_READER_BUFFER_SIZE 4096


namespace rpgss {
    namespace script {

        namespace {

            //---------------------------------------------------------
            struct _io_context {
                core::ByteArray::Ptr buffer;
                io::File::Ptr stream;

                explicit _io_context(io::File* s)
                    : stream(s)
                {
                    buffer = core::ByteArray::New(RPGSS_READER_BUFFER_SIZE);
                }

                ~_io_context() {
                }
            };

            //---------------------------------------------------------
            const char* _reader(lua_State* L, void* userData, size_t* out_size)
            {
                _io_context* c = (_io_context*)userData;

                int n = c->stream->read(c->buffer->getBuffer(), RPGSS_READER_BUFFER_SIZE);

                if (n > 0) {
                    // ok
                    *out_size = (size_t)n;
                    return (const char*)c->buffer->getBuffer();
                } else {
                    // error
                    *out_size = 0;
                    return 0;
                }
            }

            //---------------------------------------------------------
            int _writer(lua_State* L, const void* buffer, size_t bufferSize, void* userData)
            {
                // allow writing empty buffers
                if (bufferSize == 0) {
                    return 0;
                }

                _io_context* c = (_io_context*)userData;

                int n = c->stream->write((u8*)buffer, bufferSize);

                if (n > 0) {
                    return 0; // ok
                } else {
                    return 1; // error
                }
            }

        } // anonymous namespace

        //---------------------------------------------------------
        bool RegisterModules(lua_State* L)
        {
            return (
                audio_module::RegisterAudioModule(L) &&
                core_module::RegisterCoreModule(L) &&
                game_module::RegisterGameModule(L) &&
                graphics_module::RegisterGraphicsModule(L) &&
                io_module::RegisterIoModule(L) &&
                keyboard_module::RegisterKeyboardModule(L) &&
                mouse_module::RegisterMouseModule(L) &&
                timer_module::RegisterTimerModule(L) &&
                util_module::RegisterUtilModule(L)
            );
        }

        //---------------------------------------------------------
        bool DoFile(lua_State* L, const std::string& filename)
        {
            assert(L != 0);
            assert(!filename.empty());
            lua_getglobal(L, "dofile");
            lua_pushstring(L, filename.c_str());
            return Call(L, 1, 0);
        }

        //---------------------------------------------------------
        bool Call(lua_State* L, int nargs, int nresults)
        {
            return luax_pcall(L, nargs, nresults) == LUA_OK;
        }

        //---------------------------------------------------------
        int luax_panic_handler(lua_State* L)
        {
            const char* error_message = "(N/A)";
            if (lua_isstring(L, -1)) {
                error_message = lua_tostring(L, -1);
            }

            luaL_traceback(L, L, 0, 2);
            const char* stack_trace = lua_tostring(L, -1);

            debug::Log() << "Fatal error:";
            debug::Log(1) << error_message;
            debug::Log() << stack_trace;

            debug::ShowError(
                "Lua error occurred outside of a protected call:\n\n%s\n\nSee log file for more information.",
                error_message
            );

            return 0; // returning from this handler will result in program abortion
        }

        //---------------------------------------------------------
        int luax_message_handler(lua_State* L)
        {
            if (lua_isstring(L, -1)) {
                // see if the string already has stack trace appended, if so just return it unmodified
                if (std::string(lua_tostring(L, 1)).find(RPGSS_STACK_TRACE_MAGIC) != std::string::npos) {
                    return 1;
                }
            }

            lua_pushstring(L, RPGSS_STACK_TRACE_MAGIC);
            luaL_traceback(L, L, 0, 2);
            lua_concat(L, 3);

            return 1;
        }

        //---------------------------------------------------------
        int luax_pcall(lua_State* L, int nargs, int nresults)
        {
            int msgh_index = lua_gettop(L) - nargs;
            lua_pushcfunction(L, luax_message_handler);
            lua_insert(L, msgh_index); // move message handler before chunk and args
            int result = lua_pcall(L, nargs, nresults, msgh_index);
            lua_remove(L, msgh_index); // remove message handler
            return result;
        }

        //---------------------------------------------------------
        int luax_print(lua_State* L)
        {
            { // wrap following code in a block, so our stack variables will be properly destroyed on errors

                // get number of arguments
                int n = lua_gettop(L);
                if (n == 0) {
                    // nothing to print
                    return 0;
                }

                // prepare tostring function
                lua_getglobal(L, "tostring");
                if (lua_isfunction(L, -1) == 0) {
                    lua_pushstring(L, "global 'tostring' is not a function");
                    goto error;
                }

                // stringify arguments
                std::ostringstream oss;
                for (int i = 1; i <= n; i++) {
                    lua_pushvalue(L, -1); // push tostring function
                    lua_pushvalue(L, i); // push argument
                    if (luax_pcall(L, 1, 1) != LUA_OK) { // call tostring(argument)
                        goto throw_error;
                    }

                    // make sure we actually got a string
                    if (lua_isstring(L, -1) == 0) {
                        lua_pushstring(L, "return value of 'tostring' must be a string");
                        goto error;
                    }

                    oss << lua_tostring(L, -1);
                    if (i != n) {
                        oss << ", ";
                    }

                    lua_pop(L, 1); // pop result of tostring(argument)
                }

                debug::ShowMessage(oss.str().c_str());
            }

            return 0;

        error:
            luaL_where(L, 0);
            lua_insert(L, -2);
            lua_concat(L, 2);

        throw_error:
            return lua_error(L);
        }

        //---------------------------------------------------------
        int luax_loadfile(lua_State* L)
        {
            // make sure filename argument is a string
            luaL_checkstring(L, 1);

            { // wrap following code in a block, so our stack variables will be properly destroyed on errors

                std::string filename = lua_tostring(L, 1);
                if (filename.empty()) {
                    lua_pushstring(L, "empty filename");
                    goto error;
                }

                std::string filepath = "Scripts/" + ReplaceString(filename, ".", "/");
                std::string filepath_lua;
                std::string filepath_luac;

                if (io::IsDirectory(filepath)) {
                    filepath_lua  = filepath + "/init.lua";
                    filepath_luac = filepath + "/init.luac";
                } else {
                    filepath_lua  = filepath + ".lua";
                    filepath_luac = filepath + ".luac";
                }

                // see if a bytecode version of the file exists
                if (io::Exists(filepath_luac)) {
                    // if a plain text version of the file also exists,
                    // see if the bytecode version is outdated
                    if (io::Exists(filepath_lua) &&
                        io::LastWriteTime(filepath_lua) > io::LastWriteTime(filepath_luac))
                    {
                        // bytecode version outdated, load the more up to date plain text version
                        filepath = filepath_lua;
                    }
                    else // no plain text version exists, or bytecode version is up to date
                    {
                        filepath = filepath_luac;
                    }
                } else { // bycode version does not exist
                    filepath = filepath_lua;
                }

                // make sure file exists
                if (!io::Exists(filepath)) {
                    lua_pushstring(L, "file '");
                    lua_pushstring(L, filepath.c_str());
                    lua_pushstring(L, "' does not exist");
                    lua_concat(L, 3);
                    goto error;
                }

                // open input stream
                io::File::Ptr is = io::OpenFile(filepath);
                if (!is) {
                    lua_pushstring(L, "could not open file '");
                    lua_pushstring(L, filepath.c_str());
                    lua_pushstring(L, "'");
                    lua_concat(L, 3);
                    goto error;
                }

                // load chunk
                _io_context ictx(is);
                int result = lua_load(L, _reader, &ictx, ("@" + filepath).c_str());
                if (result != LUA_OK) {
                    if (result == LUA_ERRSYNTAX) {
                        lua_pushstring(L, "syntax error: ");
                        lua_insert(L, -2);
                        lua_concat(L, 2);
                    }
                    goto error;
                }

                // automatically create bytecode versions of plain text files, silently ignore any errors
                if (filepath.rfind(".lua") == filepath.size() - 4) {
                    io::File::Ptr os = io::OpenFile(filepath + "c", io::File::Out);
                    if (os) {
                        _io_context octx(os);
                        lua_dump(L, _writer, &octx);
                    }
                }
            }

            return 1;

        error:
            luaL_where(L, 0);
            lua_insert(L, -2);
            lua_concat(L, 2);

        throw_error:
            return lua_error(L);
        }

        //---------------------------------------------------------
        int luax_dofile(lua_State* L)
        {
            int oldtop = lua_gettop(L);

            // make sure filename argument is a string
            luaL_checkstring(L, 1);

            // load chunk
            lua_pushcfunction(L, luax_loadfile);
            lua_pushvalue(L, 1);
            if (luax_pcall(L, 1, 1) != LUA_OK) {
                return lua_error(L);
            }

            // execute chunk
            if (luax_pcall(L, 0, LUA_MULTRET) != LUA_OK) {
                return lua_error(L);
            }

            return lua_gettop(L) - oldtop;
        }

    } // namespace script
} // namespace rpgss
