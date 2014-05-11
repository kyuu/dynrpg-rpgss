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
