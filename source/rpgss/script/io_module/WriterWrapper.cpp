#include <cassert>

#include "../../Context.hpp"
#include "../core_module/core_module.hpp"
#include "WriterWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace io_module {

            //---------------------------------------------------------
            void
            WriterWrapper::Push(lua_State* L, io::Writer* binarywriter)
            {
                assert(binarywriter);
                luabridge::push(L, WriterWrapper(binarywriter));
            }

            //---------------------------------------------------------
            bool
            WriterWrapper::Is(lua_State* L, int index)
            {
                assert(index != 0);

                if (index < 0) { // allow negative indices
                    index = lua_gettop(L) + index + 1;
                }

                return luabridge::Stack<WriterWrapper*>::is_a(L, index);
            }

            //---------------------------------------------------------
            io::Writer*
            WriterWrapper::Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "Writer expected, got nothing");
                    return 0;
                }
                WriterWrapper* wrapper = luabridge::Stack<WriterWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "Writer expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            io::Writer*
            WriterWrapper::GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                WriterWrapper* wrapper = luabridge::Stack<WriterWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            WriterWrapper::WriterWrapper(io::Writer* ptr)
                : This(ptr)
            {
            }

            //---------------------------------------------------------
            FileWrapper
            WriterWrapper::get_outputStream() const
            {
                return FileWrapper(This->getOutputStream());
            }

            //---------------------------------------------------------
            int
            WriterWrapper::writeByte(lua_State* L)
            {
                int byte = luaL_checkint(L, 2);
                int count = luaL_optint(L, 3, 1);
                if (count <= 0) {
                    return luaL_error(L, "invalid count");
                }
                This->writeByte(byte, count);
                return 0;
            }

            //---------------------------------------------------------
            int
            WriterWrapper::writeBytes(lua_State* L)
            {
                core::ByteArray* byte_array = core_module::ByteArrayWrapper::Get(L, 2);
                This->writeByteArray(byte_array);
                return 0;
            }

            //---------------------------------------------------------
            int
            WriterWrapper::writeString(lua_State* L)
            {
                const char* s = luaL_checkstring(L, 2);
                This->writeString(s);
                return 0;
            }

            //---------------------------------------------------------
            int
            WriterWrapper::writeBool(lua_State* L)
            {
                luaL_argcheck(L, lua_isnumber(L, 2) || lua_isboolean(L, 2), 2, "expected a number or boolean");
                bool b = lua_toboolean(L, 2);
                This->writeBool(b);
                return 0;
            }

            //---------------------------------------------------------
            int
            WriterWrapper::writeInt8(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                This->writeInt8(n);
                return 0;
            }

            //---------------------------------------------------------
            int
            WriterWrapper::writeInt16(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                This->writeInt16(n);
                return 0;
            }

            //---------------------------------------------------------
            int
            WriterWrapper::writeInt16BE(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                This->writeInt16BE(n);
                return 0;
            }

            //---------------------------------------------------------
            int
            WriterWrapper::writeInt32(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                This->writeInt32(n);
                return 0;
            }

            //---------------------------------------------------------
            int
            WriterWrapper::writeInt32BE(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                This->writeInt32BE(n);
                return 0;
            }

            //---------------------------------------------------------
            int
            WriterWrapper::writeUint8(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                This->writeUint8(n);
                return 0;
            }

            //---------------------------------------------------------
            int
            WriterWrapper::writeUint16(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                This->writeUint16(n);
                return 0;
            }

            //---------------------------------------------------------
            int
            WriterWrapper::writeUint16BE(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                This->writeUint16BE(n);
                return 0;
            }

            //---------------------------------------------------------
            int
            WriterWrapper::writeUint32(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                This->writeUint32(n);
                return 0;
            }

            //---------------------------------------------------------
            int
            WriterWrapper::writeUint32BE(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                This->writeUint32BE(n);
                return 0;
            }

            //---------------------------------------------------------
            int
            WriterWrapper::writeFloat(lua_State* L)
            {
                float n = luaL_checknumber(L, 2);
                This->writeFloat(n);
                return 0;
            }

            //---------------------------------------------------------
            int
            WriterWrapper::writeFloatBE(lua_State* L)
            {
                float n = luaL_checknumber(L, 2);
                This->writeFloatBE(n);
                return 0;
            }

            //---------------------------------------------------------
            int
            WriterWrapper::writeDouble(lua_State* L)
            {
                double n = luaL_checknumber(L, 2);
                This->writeDouble(n);
                return 0;
            }

            //---------------------------------------------------------
            int
            WriterWrapper::writeDoubleBE(lua_State* L)
            {
                double n = luaL_checknumber(L, 2);
                This->writeDoubleBE(n);
                return 0;
            }

        } // io_module
    } // namespace script
} // namespace rpgss
