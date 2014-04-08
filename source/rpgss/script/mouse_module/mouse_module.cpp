#include "mouse_module.hpp"


namespace rpgss {
    namespace script {
        namespace mouse_module {

            //---------------------------------------------------------
            int mouse_get_x()
            {
                return input::GetMousePosition().x;
            }

            //---------------------------------------------------------
            int mouse_get_y()
            {
                return input::GetMousePosition().y;
            }

            //---------------------------------------------------------
            int mouse_getPosition(lua_State* L)
            {
                core::Vec2i pos = input::GetMousePosition();
                lua_pushnumber(L, pos.x);
                lua_pushnumber(L, pos.y);
                return 2;
            }

            //---------------------------------------------------------
            int mouse_isDown(lua_State* L)
            {
                int nargs = lua_gettop(L);
                bool result = false;
                for (int i = 1; i <= nargs; i++) {
                    const char* mbutton_str = luaL_checkstring(L, i);
                    int mbutton;
                    if (!GetMouseButtonConstant(mbutton_str, mbutton)) {
                        return luaL_argerror(L, i, mbutton_str);
                    }
                    result |= input::IsMouseButtonPressed(mbutton);
                }
                lua_pushboolean(L, result);
                return 1;
            }

            //---------------------------------------------------------
            int mouse_isAnyDown(lua_State* L)
            {
                lua_pushboolean(L, input::IsAnyMouseButtonPressed());
                return 1;
            }

            //---------------------------------------------------------
            bool RegisterMouseModule(lua_State* L)
            {
                luabridge::getGlobalNamespace(L)
                    .beginNamespace("mouse")

                        .addProperty("x",            &mouse_get_x)
                        .addProperty("y",            &mouse_get_y)
                        .addCFunction("getPosition", &mouse_getPosition)
                        .addCFunction("isDown",      &mouse_isDown)
                        .addCFunction("isAnyDown",   &mouse_isAnyDown)

                    .endNamespace();

                return true;
            }

        } // mouse_module
    } // namespace script
} // namespace rpgss
