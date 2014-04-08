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
