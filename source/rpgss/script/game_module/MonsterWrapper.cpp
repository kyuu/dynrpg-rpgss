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

#define NOT_MAIN_MODULE
#include <DynRPG/DynRPG.h>

#include "../../Context.hpp"
#include "MonsterWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace game_module {

            //---------------------------------------------------------
            void
            MonsterWrapper::Push(lua_State* L, RPG::Monster* monster)
            {
                assert(monster);
                luabridge::push(L, MonsterWrapper(monster));
            }

            //---------------------------------------------------------
            bool
            MonsterWrapper::Is(lua_State* L, int index)
            {
                assert(index != 0);

                if (index < 0) { // allow negative indices
                    index = lua_gettop(L) + index + 1;
                }

                return luabridge::Stack<MonsterWrapper*>::is_a(L, index);
            }

            //---------------------------------------------------------
            RPG::Monster*
            MonsterWrapper::Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "Monster expected, got nothing");
                    return 0;
                }
                MonsterWrapper* wrapper = luabridge::Stack<MonsterWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "Monster expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            RPG::Monster*
            MonsterWrapper::GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                MonsterWrapper* wrapper = luabridge::Stack<MonsterWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            MonsterWrapper::MonsterWrapper(RPG::Monster* ptr)
                : BattlerWrapper(ptr)
                , This(ptr)
            {
            }

            //---------------------------------------------------------
            int
            MonsterWrapper::get_monsterPartyId() const
            {
                return This->id + 1;
            }

        } // namespace game_module
    } // namespace script
} // namespace rpgss
