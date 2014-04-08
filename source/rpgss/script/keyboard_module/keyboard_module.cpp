#include "keyboard_module.hpp"


namespace rpgss {
    namespace script {
        namespace keyboard_module {

            //---------------------------------------------------------
            int keyboard_isDown(lua_State* L)
            {
                int nargs = lua_gettop(L);
                bool result = false;
                for (int i = 1; i <= nargs; i++) {
                    const char* key_str = luaL_checkstring(L, i);
                    int key;
                    if (!GetKeyConstant(key_str, key)) {
                        return luaL_argerror(L, i, key_str);
                    }
                    result |= input::IsKeyPressed(key);
                }
                lua_pushboolean(L, result);
                return 1;
            }

            //---------------------------------------------------------
            int keyboard_isAnyDown(lua_State* L)
            {
                lua_pushboolean(L, input::IsAnyKeyPressed());
                return 1;
            }

            //---------------------------------------------------------
            bool RegisterKeyboardModule(lua_State* L)
            {
                luabridge::getGlobalNamespace(L)
                    .beginNamespace("keyboard")

                        .addCFunction("isDown",    &keyboard_isDown)
                        .addCFunction("isAnyDown", &keyboard_isAnyDown)

                    .endNamespace();

                return true;
            }

        } // keyboard_module
    } // namespace script
} // namespace rpgss
