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
#include "ActorWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace game_module {

            //---------------------------------------------------------
            void
            ActorWrapper::Push(lua_State* L, RPG::Actor* actor)
            {
                assert(actor);
                luabridge::push(L, ActorWrapper(actor));
            }

            //---------------------------------------------------------
            bool
            ActorWrapper::Is(lua_State* L, int index)
            {
                assert(index != 0);

                if (index < 0) { // allow negative indices
                    index = lua_gettop(L) + index + 1;
                }

                return luabridge::Stack<ActorWrapper*>::is_a(L, index);
            }

            //---------------------------------------------------------
            RPG::Actor*
            ActorWrapper::Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "Actor expected, got nothing");
                    return 0;
                }
                ActorWrapper* wrapper = luabridge::Stack<ActorWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "Actor expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            RPG::Actor*
            ActorWrapper::GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                ActorWrapper* wrapper = luabridge::Stack<ActorWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            ActorWrapper::ActorWrapper(RPG::Actor* ptr)
                : BattlerWrapper(ptr)
                , This(ptr)
            {
            }

            //---------------------------------------------------------
            std::string
            ActorWrapper::get_degree() const
            {
                return const_cast<RPG::Actor*>(This)->getDegree();
            }

            //---------------------------------------------------------
            std::string
            ActorWrapper::get_charsetFilename() const
            {
                return const_cast<RPG::Actor*>(This)->getCharsetFilename();
            }

            //---------------------------------------------------------
            int
            ActorWrapper::get_charsetId() const
            {
                return const_cast<RPG::Actor*>(This)->getCharsetId();
            }

            //---------------------------------------------------------
            std::string
            ActorWrapper::get_facesetFilename() const
            {
                return const_cast<RPG::Actor*>(This)->getFacesetFilename();
            }

            //---------------------------------------------------------
            int
            ActorWrapper::get_facesetId() const
            {
                return const_cast<RPG::Actor*>(This)->getFacesetId();
            }

            //---------------------------------------------------------
            int
            ActorWrapper::get_level() const
            {
                return const_cast<RPG::Actor*>(This)->level;
            }

            //---------------------------------------------------------
            void
            ActorWrapper::set_level(int level)
            {
                if (level < 1 || level > 99) {
                    luaL_error(Context::Current().interpreter(), "invalid value");
                }
                This->level = level;
            }

            //---------------------------------------------------------
            int
            ActorWrapper::get_exp() const
            {
                return const_cast<RPG::Actor*>(This)->exp;
            }

            //---------------------------------------------------------
            void
            ActorWrapper::set_exp(int exp)
            {
                if (exp < 0) {
                    luaL_error(Context::Current().interpreter(), "invalid value");
                }
                This->exp = exp;
            }

            //---------------------------------------------------------
            int
            ActorWrapper::get_weaponId() const
            {
                return const_cast<RPG::Actor*>(This)->weaponId;
            }

            //---------------------------------------------------------
            void
            ActorWrapper::set_weaponId(int weaponId)
            {
                if (weaponId < 0) {
                    luaL_error(Context::Current().interpreter(), "invalid value");
                }
                This->weaponId = weaponId;
            }

            //---------------------------------------------------------
            int
            ActorWrapper::get_shieldId() const
            {
                return const_cast<RPG::Actor*>(This)->shieldId;
            }

            //---------------------------------------------------------
            void
            ActorWrapper::set_shieldId(int shieldId)
            {
                if (shieldId < 0) {
                    luaL_error(Context::Current().interpreter(), "invalid value");
                }
                This->shieldId = shieldId;
            }

            //---------------------------------------------------------
            int
            ActorWrapper::get_armorId() const
            {
                return const_cast<RPG::Actor*>(This)->armorId;
            }

            //---------------------------------------------------------
            void
            ActorWrapper::set_armorId(int armorId)
            {
                if (armorId < 0) {
                    luaL_error(Context::Current().interpreter(), "invalid value");
                }
                This->armorId = armorId;
            }

            //---------------------------------------------------------
            int
            ActorWrapper::get_helmetId() const
            {
                return const_cast<RPG::Actor*>(This)->helmetId;
            }

            //---------------------------------------------------------
            void
            ActorWrapper::set_helmetId(int helmetId)
            {
                if (helmetId < 0) {
                    luaL_error(Context::Current().interpreter(), "invalid value");
                }
                This->helmetId = helmetId;
            }

            //---------------------------------------------------------
            int
            ActorWrapper::get_accessoryId() const
            {
                return const_cast<RPG::Actor*>(This)->accessoryId;
            }

            //---------------------------------------------------------
            void
            ActorWrapper::set_accessoryId(int accessoryId)
            {
                if (accessoryId < 0) {
                    luaL_error(Context::Current().interpreter(), "invalid value");
                }
                This->accessoryId = accessoryId;
            }

            //---------------------------------------------------------
            bool
            ActorWrapper::get_twoWeapons() const
            {
                return const_cast<RPG::Actor*>(This)->twoWeapons;
            }

            //---------------------------------------------------------
            void
            ActorWrapper::set_twoWeapons(bool twoWeapons)
            {
                This->twoWeapons = twoWeapons;
            }

            //---------------------------------------------------------
            bool
            ActorWrapper::get_lockEquipment() const
            {
                return const_cast<RPG::Actor*>(This)->lockEquipemnt;
            }

            //---------------------------------------------------------
            void
            ActorWrapper::set_lockEquipment(bool lockEquipment)
            {
                This->lockEquipemnt = lockEquipment;
            }

            //---------------------------------------------------------
            bool
            ActorWrapper::get_autoBattle() const
            {
                return const_cast<RPG::Actor*>(This)->autoBattle;
            }

            //---------------------------------------------------------
            void
            ActorWrapper::set_autoBattle(bool autoBattle)
            {
                This->autoBattle = autoBattle;
            }

            //---------------------------------------------------------
            bool
            ActorWrapper::get_mightyGuard() const
            {
                return const_cast<RPG::Actor*>(This)->mightyGuard;
            }

            //---------------------------------------------------------
            void
            ActorWrapper::set_mightyGuard(bool mightyGuard)
            {
                This->mightyGuard = mightyGuard;
            }

        } // namespace game_module
    } // namespace script
} // namespace rpgss
