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
            int mouse_getState(lua_State* L)
            {
                // create a table with enough space allocated
                // to hold the state for all mouse buttons
                lua_createtable(L, 0, input::NUMMOUSEBUTTONS);

                // populate table with key states
                std::string mbutton_str;
                for (int mbutton = 0; mbutton < input::NUMMOUSEBUTTONS; mbutton++) {
                    GetMouseButtonConstant(mbutton, mbutton_str);
                    lua_pushlstring(L, mbutton_str.c_str(), mbutton_str.size()); // push mouse button constant
                    lua_pushboolean(L, input::IsMouseButtonPressed(mbutton)); // push mouse button state
                    lua_rawset(L, -3); // set in table
                }

                return 1;
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
                        .addCFunction("getState",    &mouse_getState)
                        .addCFunction("isDown",      &mouse_isDown)
                        .addCFunction("isAnyDown",   &mouse_isAnyDown)

                    .endNamespace();

                return true;
            }

        } // mouse_module
    } // namespace script
} // namespace rpgss
