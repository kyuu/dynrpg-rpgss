#include <cassert>

#include "../../Context.hpp"
#include "../core_module/core_module.hpp"
#include "MemoryFileWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace io_module {

            //---------------------------------------------------------
            void
            MemoryFileWrapper::Push(lua_State* L, io::MemoryFile* memorystream)
            {
                assert(memorystream);
                luabridge::push(L, MemoryFileWrapper(memorystream));
            }

            //---------------------------------------------------------
            bool
            MemoryFileWrapper::Is(lua_State* L, int index)
            {
                assert(index != 0);

                if (index < 0) { // allow negative indices
                    index = lua_gettop(L) + index + 1;
                }

                return luabridge::Stack<MemoryFileWrapper*>::is_a(L, index);
            }

            //---------------------------------------------------------
            io::MemoryFile*
            MemoryFileWrapper::Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "MemoryFile expected, got nothing");
                    return 0;
                }
                MemoryFileWrapper* wrapper = luabridge::Stack<MemoryFileWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "MemoryFile expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            io::MemoryFile*
            MemoryFileWrapper::GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                MemoryFileWrapper* wrapper = luabridge::Stack<MemoryFileWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            MemoryFileWrapper::MemoryFileWrapper(io::MemoryFile* ptr)
                : FileWrapper(ptr)
                , This(ptr)
            {
            }

            //---------------------------------------------------------
            int
            MemoryFileWrapper::__len(lua_State* L)
            {
                lua_pushnumber(L, This->getSize());
                return 1;
            }

            //---------------------------------------------------------
            bool
            MemoryFileWrapper::get_capacity() const
            {
                return This->getCapacity();
            }

            //---------------------------------------------------------
            int
            MemoryFileWrapper::copyBuffer(lua_State* L)
            {
                core::ByteArray::Ptr bytearray = core::ByteArray::New(This->getBuffer(), This->getSize());
                core_module::ByteArrayWrapper::Push(L, bytearray);
                return 1;
            }

            //---------------------------------------------------------
            int
            MemoryFileWrapper::reserve(lua_State* L)
            {
                int capacity = luaL_checkint(L, 2);

                luaL_argcheck(L, capacity >= 0, 2, "invalid capacity");

                This->reserve(capacity);
                return 0;
            }

            //---------------------------------------------------------
            int
            MemoryFileWrapper::clear(lua_State* L)
            {
                This->clear();
                return 0;
            }

        } // io_module
    } // namespace script
} // namespace rpgss
