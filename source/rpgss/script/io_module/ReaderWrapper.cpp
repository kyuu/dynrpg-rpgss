#include <cassert>

#include "../../Context.hpp"
#include "../core_module/core_module.hpp"
#include "ReaderWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace io_module {

            //---------------------------------------------------------
            void
            ReaderWrapper::Push(lua_State* L, io::Reader* binaryreader)
            {
                assert(binaryreader);
                luabridge::push(L, ReaderWrapper(binaryreader));
            }

            //---------------------------------------------------------
            bool
            ReaderWrapper::Is(lua_State* L, int index)
            {
                assert(index != 0);

                if (index < 0) { // allow negative indices
                    index = lua_gettop(L) + index + 1;
                }

                return luabridge::Stack<ReaderWrapper*>::is_a(L, index);
            }

            //---------------------------------------------------------
            io::Reader*
            ReaderWrapper::Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "Reader expected, got nothing");
                    return 0;
                }
                ReaderWrapper* wrapper = luabridge::Stack<ReaderWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "Reader expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            io::Reader*
            ReaderWrapper::GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                ReaderWrapper* wrapper = luabridge::Stack<ReaderWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            ReaderWrapper::ReaderWrapper(io::Reader* ptr)
                : This(ptr)
            {
            }

            //---------------------------------------------------------
            FileWrapper
            ReaderWrapper::get_inputStream() const
            {
                return FileWrapper(This->getInputStream());
            }

            //---------------------------------------------------------
            int
            ReaderWrapper::skip(lua_State* L)
            {
                int count = luaL_checkint(L, 2);
                if (count < 0) {
                    return luaL_error(L, "invalid count");
                }
                This->skipBytes(count);
                return 0;
            }

            //---------------------------------------------------------
            int
            ReaderWrapper::readByte(lua_State* L)
            {
                u8 byte = This->readByte();
                lua_pushnumber(L, byte);
                return 1;
            }

            //---------------------------------------------------------
            int
            ReaderWrapper::readBytes(lua_State* L)
            {
                int count = luaL_checkint(L, 2);

                luaL_argcheck(L, count >= 0, 2, "invalid count");

                core::ByteArray::Ptr bytearray = This->readByteArray(count);
                core_module::ByteArrayWrapper::Push(L, bytearray);
                return 1;
            }

            //---------------------------------------------------------
            int
            ReaderWrapper::readString(lua_State* L)
            {
                int size = luaL_checkint(L, 2);

                luaL_argcheck(L, size >= 0, 2, "invalid size");

                std::string s = This->readString(size);
                lua_pushlstring(L, s.c_str(), s.size());
                return 1;
            }

            //---------------------------------------------------------
            int
            ReaderWrapper::readBool(lua_State* L)
            {
                bool b = This->readBool();
                lua_pushboolean(L, b);
                return 1;
            }

            //---------------------------------------------------------
            int
            ReaderWrapper::readInt8(lua_State* L)
            {
                i8 n = This->readInt8();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int
            ReaderWrapper::readInt16(lua_State* L)
            {
                i16 n = This->readInt16();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int
            ReaderWrapper::readInt16BE(lua_State* L)
            {
                i16 n = This->readInt16BE();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int
            ReaderWrapper::readInt32(lua_State* L)
            {
                i32 n = This->readInt32();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int
            ReaderWrapper::readInt32BE(lua_State* L)
            {
                i32 n = This->readInt32BE();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int
            ReaderWrapper::readUint8(lua_State* L)
            {
                u8 n = This->readUint8();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int
            ReaderWrapper::readUint16(lua_State* L)
            {
                u16 n = This->readUint16();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int
            ReaderWrapper::readUint16BE(lua_State* L)
            {
                u16 n = This->readUint16BE();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int
            ReaderWrapper::readUint32(lua_State* L)
            {
                u32 n = This->readUint32();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int
            ReaderWrapper::readUint32BE(lua_State* L)
            {
                u32 n = This->readUint32BE();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int
            ReaderWrapper::readFloat(lua_State* L)
            {
                float n = This->readFloat();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int
            ReaderWrapper::readFloatBE(lua_State* L)
            {
                float n = This->readFloatBE();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int
            ReaderWrapper::readDouble(lua_State* L)
            {
                double n = This->readDouble();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int
            ReaderWrapper::readDoubleBE(lua_State* L)
            {
                double n = This->readDoubleBE();
                lua_pushnumber(L, n);
                return 1;
            }

        } // io_module
    } // namespace script
} // namespace rpgss
