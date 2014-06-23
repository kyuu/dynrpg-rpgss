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

#include "keyboard_module.hpp"


namespace rpgss {
    namespace script {
        namespace keyboard_module {

            //---------------------------------------------------------
            int keyboard_getVirtualKeyCode(lua_State* L)
            {
                const char* key_str = luaL_checkstring(L, 1);

                // convert key constant to key code
                int key;
                if (!GetKeyConstant(key_str, key)) {
                    return luaL_argerror(L, 1, key_str);
                }

                // convert key code to virtual key code
                int vkey = input::GetVirtualKeyCode(key);
                lua_pushinteger(L, vkey);

                return 1;
            }

            //---------------------------------------------------------
            int keyboard_getState(lua_State* L)
            {
                // create a table with enough space allocated
                // to hold the state for all keys
                lua_createtable(L, 0, input::NUMKEYS);

                // populate table with key states
                std::string key_str;
                for (int key = 0; key < input::NUMKEYS; key++) {
                    GetKeyConstant(key, key_str);
                    lua_pushlstring(L, key_str.c_str(), key_str.size()); // push key constant
                    lua_pushboolean(L, input::IsKeyPressed(key)); // push key state
                    lua_rawset(L, -3); // set in table
                }

                return 1;
            }

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

                        .addCFunction("getVirtualKeyCode",  &keyboard_getVirtualKeyCode)
                        .addCFunction("getState",           &keyboard_getState)
                        .addCFunction("isDown",             &keyboard_isDown)
                        .addCFunction("isAnyDown",          &keyboard_isAnyDown)

                    .endNamespace();

                return true;
            }

        } // keyboard_module
    } // namespace script
} // namespace rpgss
