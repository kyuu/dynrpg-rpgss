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

#include <cstring>

#include "../../Context.hpp"
#include "../core_module/core_module.hpp"
#include "io_module.hpp"


namespace rpgss {
    namespace script {
        namespace io_module {

            //---------------------------------------------------------
            int io_newMemoryFile(lua_State* L)
            {
                int nargs = lua_gettop(L);
                if (nargs == 0)
                {
                    // newMemoryFile()
                    io::MemoryFile::Ptr memory_file = io::MemoryFile::New();
                    MemoryFileWrapper::Push(L, memory_file);
                }
                else if (nargs >= 1 && lua_isnumber(L, 1))
                {
                    // newMemoryFile(capacity [, value])
                    int capacity = luaL_checkint(L, 1);
                    int value = luaL_optint(L, 2, 0);
                    io::MemoryFile::Ptr memory_file = io::MemoryFile::New(capacity, value);
                    MemoryFileWrapper::Push(L, memory_file);
                }
                else
                {
                    // newMemoryFile(bytearray)
                    core::ByteArray* byte_array = core_module::ByteArrayWrapper::Get(L, 1);
                    io::MemoryFile::Ptr memory_file = io::MemoryFile::New(byte_array);
                    MemoryFileWrapper::Push(L, memory_file);
                }
                return 1;
            }

            //---------------------------------------------------------
            int io_newReader(lua_State* L)
            {
                io::File* file = FileWrapper::Get(L, 1);
                io::Reader::Ptr reader = io::Reader::New(file);
                ReaderWrapper::Push(L, reader);
                return 1;
            }

            //---------------------------------------------------------
            int io_newWriter(lua_State* L)
            {
                io::File* file = FileWrapper::Get(L, 1);
                io::Writer::Ptr writer = io::Writer::New(file);
                WriterWrapper::Push(L, writer);
                return 1;
            }

            //---------------------------------------------------------
            int io_openFile(lua_State* L)
            {
                const char* filename = luaL_checkstring(L, 1);
                const char* open_mode_str = luaL_optstring(L, 2, "in");
                if (std::strlen(filename) == 0) {
                    return luaL_error(L, "empty filename");
                }
                int open_mode;
                if (!GetOpenModeConstant(open_mode_str, open_mode)) {
                    return luaL_error(L, "invalid open mode constant '%s'", open_mode_str);
                }
                io::File::Ptr file = io::OpenFile(filename, (io::File::OpenMode)open_mode);
                if (file) {
                    FileWrapper::Push(L, file);
                } else {
                    lua_pushnil(L);
                }
                return 1;
            }

            //---------------------------------------------------------
            int io_makeDirectory(lua_State* L)
            {
                const char* dirname = luaL_checkstring(L, 1);
                if (std::strlen(dirname) == 0) {
                    return luaL_error(L, "empty directory name");
                }
                lua_pushboolean(L, io::MakeDirectory(dirname));
                return 1;
            }

            //---------------------------------------------------------
            int io_remove(lua_State* L)
            {
                const char* filename = luaL_checkstring(L, 1);
                if (std::strlen(filename) == 0) {
                    return luaL_error(L, "empty filename");
                }
                lua_pushboolean(L, io::Remove(filename));
                return 1;
            }

            //---------------------------------------------------------
            int io_enumerate(lua_State* L)
            {
                const char* dirname = luaL_checkstring(L, 1);
                if (std::strlen(dirname) == 0) {
                    return luaL_error(L, "empty directory name");
                }
                std::vector<std::string> file_list;
                if (io::Enumerate(dirname, file_list)) {
                    luabridge::LuaRef t = luabridge::newTable(L);
                    for (size_t i = 0; i < file_list.size(); i++) {
                        t[i+1] = file_list[i];
                    }
                    luabridge::push(L, t);
                } else {
                    lua_pushnil(L);
                }
                return 1;
            }

            //---------------------------------------------------------
            int io_exists(lua_State* L)
            {
                const char* filename = luaL_checkstring(L, 1);
                if (std::strlen(filename) == 0) {
                    return luaL_error(L, "empty filename");
                }
                lua_pushboolean(L, io::Exists(filename));
                return 1;
            }

            //---------------------------------------------------------
            int io_lastWriteTime(lua_State* L)
            {
                const char* filename = luaL_checkstring(L, 1);
                if (std::strlen(filename) == 0) {
                    return luaL_error(L, "empty filename");
                }
                lua_pushnumber(L, io::LastWriteTime(filename));
                return 1;
            }

            //---------------------------------------------------------
            int io_isDirectory(lua_State* L)
            {
                const char* filename = luaL_checkstring(L, 1);
                if (std::strlen(filename) == 0) {
                    return luaL_error(L, "empty filename");
                }
                lua_pushboolean(L, io::IsDirectory(filename));
                return 1;
            }

            //---------------------------------------------------------
            int io_isFile(lua_State* L)
            {
                const char* filename = luaL_checkstring(L, 1);
                if (std::strlen(filename) == 0) {
                    return luaL_error(L, "empty filename");
                }
                lua_pushboolean(L, io::IsFile(filename));
                return 1;
            }

            //---------------------------------------------------------
            int io_mount(lua_State* L)
            {
                const char* filename = luaL_checkstring(L, 1);
                if (std::strlen(filename) == 0) {
                    return luaL_error(L, "empty filename");
                }
                lua_pushboolean(L, io::Mount(filename));
                return 1;
            }

            //---------------------------------------------------------
            int io_unmount(lua_State* L)
            {
                const char* filename = luaL_checkstring(L, 1);
                if (std::strlen(filename) == 0) {
                    return luaL_error(L, "empty filename");
                }
                lua_pushboolean(L, io::Unmount(filename));
                return 1;
            }

            //---------------------------------------------------------
            bool RegisterIoModule(lua_State* L)
            {
                luabridge::getGlobalNamespace(L)
                    .beginNamespace("io")

                        .addCFunction("makeDirectory",          &io_makeDirectory)
                        .addCFunction("remove",                 &io_remove)
                        .addCFunction("enumerate",              &io_enumerate)
                        .addCFunction("exists",                 &io_exists)
                        .addCFunction("lastWriteTime",          &io_lastWriteTime)
                        .addCFunction("isDirectory",            &io_isDirectory)
                        .addCFunction("isFile",                 &io_isFile)
                        .addCFunction("mount",                  &io_mount)
                        .addCFunction("unmount",                &io_unmount)

                        .beginClass<FileWrapper>("File")
                            .addProperty("isOpen",              &FileWrapper::get_isOpen)
                            .addProperty("size",                &FileWrapper::get_size)
                            .addProperty("eof",                 &FileWrapper::get_eof)
                            .addCFunction("__len",              &FileWrapper::__len)
                            .addCFunction("close",              &FileWrapper::close)
                            .addCFunction("seek",               &FileWrapper::seek)
                            .addCFunction("tell",               &FileWrapper::tell)
                            .addCFunction("read",               &FileWrapper::read)
                            .addCFunction("write",              &FileWrapper::write)
                            .addCFunction("flush",              &FileWrapper::flush)
                        .endClass()

                        .addCFunction("openFile", &io_openFile)

                        .deriveClass<MemoryFileWrapper, FileWrapper>("MemoryFile")
                            .addProperty("capacity",            &MemoryFileWrapper::get_capacity)
                            .addCFunction("__len",              &MemoryFileWrapper::__len)
                            .addCFunction("copyBuffer",         &MemoryFileWrapper::copyBuffer)
                            .addCFunction("reserve",            &MemoryFileWrapper::reserve)
                            .addCFunction("clear",              &MemoryFileWrapper::clear)
                        .endClass()

                        .addCFunction("newMemoryFile", &io_newMemoryFile)

                        .beginClass<ReaderWrapper>("Reader")
                            .addProperty("inputStream",         &ReaderWrapper::get_inputStream)
                            .addCFunction("skipBytes",          &ReaderWrapper::skipBytes)
                            .addCFunction("readByte",           &ReaderWrapper::readByte)
                            .addCFunction("readBytes",          &ReaderWrapper::readBytes)
                            .addCFunction("readString",         &ReaderWrapper::readString)
                            .addCFunction("readBool",           &ReaderWrapper::readBool)
                            .addCFunction("readInt8",           &ReaderWrapper::readInt8)
                            .addCFunction("readInt16",          &ReaderWrapper::readInt16)
                            .addCFunction("readInt16BE",        &ReaderWrapper::readInt16BE)
                            .addCFunction("readInt32",          &ReaderWrapper::readInt32)
                            .addCFunction("readInt32BE",        &ReaderWrapper::readInt32BE)
                            .addCFunction("readUint8",          &ReaderWrapper::readUint8)
                            .addCFunction("readUint16",         &ReaderWrapper::readUint16)
                            .addCFunction("readUint16BE",       &ReaderWrapper::readUint16BE)
                            .addCFunction("readUint32",         &ReaderWrapper::readUint32)
                            .addCFunction("readUint32BE",       &ReaderWrapper::readUint32BE)
                            .addCFunction("readFloat",          &ReaderWrapper::readFloat)
                            .addCFunction("readFloatBE",        &ReaderWrapper::readFloatBE)
                            .addCFunction("readDouble",         &ReaderWrapper::readDouble)
                            .addCFunction("readDoubleBE",       &ReaderWrapper::readDoubleBE)
                        .endClass()

                        .addCFunction("newReader", &io_newReader)

                        .beginClass<WriterWrapper>("Writer")
                            .addProperty("outputStream",        &WriterWrapper::get_outputStream)
                            .addCFunction("writeByte",          &WriterWrapper::writeByte)
                            .addCFunction("writeBytes",         &WriterWrapper::writeBytes)
                            .addCFunction("writeString",        &WriterWrapper::writeString)
                            .addCFunction("writeBool",          &WriterWrapper::writeBool)
                            .addCFunction("writeInt8",          &WriterWrapper::writeInt8)
                            .addCFunction("writeInt16",         &WriterWrapper::writeInt16)
                            .addCFunction("writeInt16BE",       &WriterWrapper::writeInt16BE)
                            .addCFunction("writeInt32",         &WriterWrapper::writeInt32)
                            .addCFunction("writeInt32BE",       &WriterWrapper::writeInt32BE)
                            .addCFunction("writeUint8",         &WriterWrapper::writeUint8)
                            .addCFunction("writeUint16",        &WriterWrapper::writeUint16)
                            .addCFunction("writeUint16BE",      &WriterWrapper::writeUint16BE)
                            .addCFunction("writeUint32",        &WriterWrapper::writeUint32)
                            .addCFunction("writeUint32BE",      &WriterWrapper::writeUint32BE)
                            .addCFunction("writeFloat",         &WriterWrapper::writeFloat)
                            .addCFunction("writeFloatBE",       &WriterWrapper::writeFloatBE)
                            .addCFunction("writeDouble",        &WriterWrapper::writeDouble)
                            .addCFunction("writeDoubleBE",      &WriterWrapper::writeDoubleBE)
                        .endClass()

                        .addCFunction("newWriter", &io_newWriter)

                    .endNamespace();

                return true;
            }

        } // io_module
    } // namespace script
} // namespace rpgss
