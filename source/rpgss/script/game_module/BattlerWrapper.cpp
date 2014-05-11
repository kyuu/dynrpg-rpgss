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
#include "BattlerWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace game_module {

            //---------------------------------------------------------
            void
            BattlerWrapper::Push(lua_State* L, RPG::Battler* battler)
            {
                assert(battler);
                luabridge::push(L, BattlerWrapper(battler));
            }

            //---------------------------------------------------------
            bool
            BattlerWrapper::Is(lua_State* L, int index)
            {
                assert(index != 0);

                if (index < 0) { // allow negative indices
                    index = lua_gettop(L) + index + 1;
                }

                return luabridge::Stack<BattlerWrapper*>::is_a(L, index);
            }

            //---------------------------------------------------------
            RPG::Battler*
            BattlerWrapper::Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "Battler expected, got nothing");
                    return 0;
                }
                BattlerWrapper* wrapper = luabridge::Stack<BattlerWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "Battler expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            RPG::Battler*
            BattlerWrapper::GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                BattlerWrapper* wrapper = luabridge::Stack<BattlerWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            BattlerWrapper::BattlerWrapper(RPG::Battler* ptr)
                : This(ptr)
            {
            }

            //---------------------------------------------------------
            int
            BattlerWrapper::get_databaseId() const
            {
                if (const_cast<RPG::Battler*>(This)->isMonster()) {
                    return static_cast<const RPG::Monster*>(This)->databaseId;
                } else {
                    return const_cast<RPG::Battler*>(This)->id;
                }
            }

            //---------------------------------------------------------
            std::string
            BattlerWrapper::get_name() const
            {
                return const_cast<RPG::Battler*>(This)->getName();
            }

            //---------------------------------------------------------
            bool
            BattlerWrapper::get_isMonster() const
            {
                return const_cast<RPG::Battler*>(This)->isMonster();
            }

            //---------------------------------------------------------
            bool
            BattlerWrapper::get_notHidden() const
            {
                return const_cast<RPG::Battler*>(This)->notHidden;
            }

            //---------------------------------------------------------
            bool
            BattlerWrapper::get_mirrored() const
            {
                return const_cast<RPG::Battler*>(This)->mirrored;
            }

            //---------------------------------------------------------
            int
            BattlerWrapper::get_hp() const
            {
                return const_cast<RPG::Battler*>(This)->hp;
            }

            //---------------------------------------------------------
            void
            BattlerWrapper::set_hp(int hp)
            {
                if (hp < 0 || hp > This->getMaxHp()) {
                    luaL_error(Context::Current().interpreter(), "invalid value");
                }
                This->hp = hp;
            }

            //---------------------------------------------------------
            int
            BattlerWrapper::get_mp() const
            {
                return const_cast<RPG::Battler*>(This)->mp;
            }

            //---------------------------------------------------------
            void
            BattlerWrapper::set_mp(int mp)
            {
                if (mp < 0 || mp > This->getMaxMp()) {
                    luaL_error(Context::Current().interpreter(), "invalid value");
                }
                This->mp = mp;
            }

            //---------------------------------------------------------
            int
            BattlerWrapper::get_maxHp() const
            {
                return const_cast<RPG::Battler*>(This)->getMaxHp();
            }

            //---------------------------------------------------------
            int
            BattlerWrapper::get_maxMp() const
            {
                return const_cast<RPG::Battler*>(This)->getMaxMp();
            }

            //---------------------------------------------------------
            int
            BattlerWrapper::get_attack() const
            {
                return const_cast<RPG::Battler*>(This)->getAttack();
            }

            //---------------------------------------------------------
            int
            BattlerWrapper::get_defense() const
            {
                return const_cast<RPG::Battler*>(This)->getDefense();
            }

            //---------------------------------------------------------
            int
            BattlerWrapper::get_intelligence() const
            {
                return const_cast<RPG::Battler*>(This)->getIntelligence();
            }

            //---------------------------------------------------------
            int
            BattlerWrapper::get_agility() const
            {
                return const_cast<RPG::Battler*>(This)->getAgility();
            }

            //---------------------------------------------------------
            int
            BattlerWrapper::get_atbValue() const
            {
                return const_cast<RPG::Battler*>(This)->atbValue;
            }

            //---------------------------------------------------------
            int
            BattlerWrapper::get_turnsTaken() const
            {
                return const_cast<RPG::Battler*>(This)->turnsTaken;
            }

            //---------------------------------------------------------
            int
            BattlerWrapper::get_screenX() const
            {
                return This->x;
            }

            //---------------------------------------------------------
            int
            BattlerWrapper::get_screenY() const
            {
                return This->y;
            }

            //---------------------------------------------------------
            int
            BattlerWrapper::getScreenPosition(lua_State* L)
            {
                lua_pushnumber(L, This->x);
                lua_pushnumber(L, This->y);
                return 2;
            }

        } // namespace game_module
    } // namespace script
} // namespace rpgss
