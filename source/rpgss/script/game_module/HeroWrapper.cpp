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
#include "constants.hpp"
#include "HeroWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace game_module {

            //---------------------------------------------------------
            void
            HeroWrapper::Push(lua_State* L, RPG::Hero* hero)
            {
                assert(hero);
                luabridge::push(L, HeroWrapper(hero));
            }

            //---------------------------------------------------------
            bool
            HeroWrapper::Is(lua_State* L, int index)
            {
                assert(index != 0);

                if (index < 0) { // allow negative indices
                    index = lua_gettop(L) + index + 1;
                }

                return luabridge::Stack<HeroWrapper*>::is_a(L, index);
            }

            //---------------------------------------------------------
            RPG::Hero*
            HeroWrapper::Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "Hero expected, got nothing");
                    return 0;
                }
                HeroWrapper* wrapper = luabridge::Stack<HeroWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "Hero expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            RPG::Hero*
            HeroWrapper::GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                HeroWrapper* wrapper = luabridge::Stack<HeroWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            HeroWrapper::HeroWrapper(RPG::Hero* ptr)
                : CharacterWrapper(ptr)
                , This(ptr)
            {
            }

            //---------------------------------------------------------
            std::string
            HeroWrapper::get_controlMode() const
            {
                std::string controlmode_str;
                if (!GetControlModeConstant(const_cast<RPG::Hero*>(This)->getControlMode(), controlmode_str)) {
                    luaL_error(Context::Current().interpreter(), "unexpected internal value");
                }
                return controlmode_str;
            }

            //---------------------------------------------------------
            void
            HeroWrapper::set_controlMode(const std::string& controlmode_str)
            {
                int controlmode;
                if (!GetControlModeConstant(controlmode_str, controlmode)) {
                    luaL_error(Context::Current().interpreter(), "invalid value");
                }
                This->setControlMode((RPG::HeroControlMode)controlmode);
            }

            //---------------------------------------------------------
            std::string
            HeroWrapper::get_vehicle() const
            {
                std::string vehicle_str;
                if (!GetVehicleConstant(const_cast<RPG::Hero*>(This)->vehicle, vehicle_str)) {
                    luaL_error(Context::Current().interpreter(), "unexpected internal value");
                }
                return vehicle_str;
            }

        } // namespace game_module
    } // namespace script
} // namespace rpgss
