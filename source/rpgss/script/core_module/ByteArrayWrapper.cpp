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
#include "ByteArrayWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace core_module {

            //---------------------------------------------------------
            void
            ByteArrayWrapper::Push(lua_State* L, core::ByteArray* bytearray)
            {
                assert(bytearray);
                luabridge::push(L, ByteArrayWrapper(bytearray));
            }

            //---------------------------------------------------------
            bool
            ByteArrayWrapper::Is(lua_State* L, int index)
            {
                assert(index != 0);

                if (index < 0) { // allow negative indices
                    index = lua_gettop(L) + index + 1;
                }

                return luabridge::Stack<ByteArrayWrapper*>::is_a(L, index);
            }

            //---------------------------------------------------------
            core::ByteArray*
            ByteArrayWrapper::Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "ByteArray expected, got nothing");
                    return 0;
                }
                ByteArrayWrapper* wrapper = luabridge::Stack<ByteArrayWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "ByteArray expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            core::ByteArray*
            ByteArrayWrapper::GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                ByteArrayWrapper* wrapper = luabridge::Stack<ByteArrayWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            ByteArrayWrapper::ByteArrayWrapper(core::ByteArray* ptr)
                : This(ptr)
            {
            }

            //---------------------------------------------------------
            int
            ByteArrayWrapper::get_size() const
            {
                return This->getSize();
            }

            //---------------------------------------------------------
            int
            ByteArrayWrapper::__len(lua_State* L)
            {
                lua_pushnumber(L, This->getSize());
                return 1;
            }

            //---------------------------------------------------------
            int
            ByteArrayWrapper::__add(lua_State* L)
            {
                core::ByteArray* rhs = ByteArrayWrapper::Get(L, 2);
                luabridge::push(L, ByteArrayWrapper(This->concat(rhs)));
                return 1;
            }

            //---------------------------------------------------------
            int
            ByteArrayWrapper::__concat(lua_State* L)
            {
                core::ByteArray* rhs = ByteArrayWrapper::Get(L, 2);
                luabridge::push(L, ByteArrayWrapper(This->concat(rhs)));
                return 1;
            }

            //---------------------------------------------------------
            int
            ByteArrayWrapper::toString(lua_State* L)
            {
                lua_pushlstring(L, (const char*)This->getBuffer(), This->getSize());
                return 1;
            }

            //---------------------------------------------------------
            int
            ByteArrayWrapper::getByte(lua_State* L)
            {
                int index = luaL_checkint(L, 2);
                if (index < 1 || index > This->getSize()) {
                    return luaL_argerror(L, 2, "index out of bounds");
                }
                lua_pushnumber(L, This->at(index - 1));
                return 1;
            }

            //---------------------------------------------------------
            int
            ByteArrayWrapper::setByte(lua_State* L)
            {
                int index = luaL_checkint(L, 2);
                int value = luaL_checkint(L, 3);
                if (index < 1 || index > This->getSize()) {
                    return luaL_argerror(L, 2, "index out of bounds");
                }
                This->at(index - 1) = value;
                return 0;
            }

            //---------------------------------------------------------
            int
            ByteArrayWrapper::resize(lua_State* L)
            {
                int size = luaL_checkint(L, 2);
                int value = luaL_optint(L, 3, 0);
                if (size < 0) {
                    return luaL_argerror(L, 2, "invalid size");
                }
                This->resize(size, value);
                return 0;
            }

            //---------------------------------------------------------
            int
            ByteArrayWrapper::memset(lua_State* L)
            {
                int value = luaL_checkint(L, 2);
                This->memset(value);
                return 0;
            }

            //---------------------------------------------------------
            int
            ByteArrayWrapper::clear(lua_State* L)
            {
                This->clear();
                return 0;
            }

        } // core_module
    } // namespace script
} // namespace rpgss
