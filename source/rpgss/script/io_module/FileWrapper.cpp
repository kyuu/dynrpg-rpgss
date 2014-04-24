#include <cassert>

#include "../../Context.hpp"
#include "../core_module/core_module.hpp"
#include "constants.hpp"
#include "FileWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace io_module {

            //---------------------------------------------------------
            void
            FileWrapper::Push(lua_State* L, io::File* file)
            {
                assert(file);
                luabridge::push(L, FileWrapper(file));
            }

            //---------------------------------------------------------
            bool
            FileWrapper::Is(lua_State* L, int index)
            {
                assert(index != 0);

                if (index < 0) { // allow negative indices
                    index = lua_gettop(L) + index + 1;
                }

                return luabridge::Stack<FileWrapper*>::is_a(L, index);
            }

            //---------------------------------------------------------
            io::File*
            FileWrapper::Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "File expected, got nothing");
                    return 0;
                }
                FileWrapper* wrapper = luabridge::Stack<FileWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "File expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            io::File*
            FileWrapper::GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                FileWrapper* wrapper = luabridge::Stack<FileWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            FileWrapper::FileWrapper(io::File* ptr)
                : This(ptr)
            {
            }

            //---------------------------------------------------------
            int
            FileWrapper::__len(lua_State* L)
            {
                lua_pushnumber(L, This->getSize());
                return 1;
            }

            //---------------------------------------------------------
            bool
            FileWrapper::get_isOpen() const
            {
                return This->isOpen();
            }

            //---------------------------------------------------------
            int
            FileWrapper::get_size() const
            {
                return This->getSize();
            }

            //---------------------------------------------------------
            bool
            FileWrapper::get_eof() const
            {
                return This->eof();
            }

            //---------------------------------------------------------
            int
            FileWrapper::close(lua_State* L)
            {
                This->close();
                return 0;
            }

            //---------------------------------------------------------
            int
            FileWrapper::seek(lua_State* L)
            {
                int offset = luaL_checkint(L, 2);
                const char* seek_mode_str = luaL_optstring(L, 3, "begin");

                int seek_mode;
                if (!GetSeekModeConstant(seek_mode_str, seek_mode)) {
                    return luaL_argerror(L, 3, "invalid seek mode constant");
                }

                lua_pushboolean(L, This->seek(offset, (io::File::SeekMode)seek_mode));
                return 1;
            }

            //---------------------------------------------------------
            int
            FileWrapper::tell(lua_State* L)
            {
                lua_pushnumber(L, This->tell());
                return 1;
            }

            //---------------------------------------------------------
            int
            FileWrapper::read(lua_State* L)
            {
                int size = luaL_checkint(L, 2);

                luaL_argcheck(L, size >= 0, 2, "invalid size");

                core::ByteArray::Ptr bytearray = core::ByteArray::New(size);
                int nread = This->read(bytearray->getBuffer(), bytearray->getSize());
                if (nread != size) {
                    bytearray->resize(nread);
                }
                core_module::ByteArrayWrapper::Push(L, bytearray);
                return 1;
            }

            //---------------------------------------------------------
            int
            FileWrapper::write(lua_State* L)
            {
                core::ByteArray* byte_array = core_module::ByteArrayWrapper::Get(L, 2);
                This->write(byte_array->getBuffer(), byte_array->getSize());
                return 0;
            }

            //---------------------------------------------------------
            int
            FileWrapper::flush(lua_State* L)
            {
                lua_pushboolean(L, This->flush());
                return 1;
            }

        } // io_module
    } // namespace script
} // namespace rpgss
