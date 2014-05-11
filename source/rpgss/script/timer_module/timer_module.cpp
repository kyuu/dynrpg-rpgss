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

#include "timer_module.hpp"


namespace rpgss {
    namespace script {
        namespace timer_module {

            //---------------------------------------------------------
            int timer_getTicks(lua_State* L)
            {
                lua_pushnumber(L, timer::GetTicks());
                return 1;
            }

            //---------------------------------------------------------
            int timer_getTime(lua_State* L)
            {
                lua_pushnumber(L, timer::GetTime());
                return 1;
            }

            //---------------------------------------------------------
            int timer_getTimeInfo(lua_State* L)
            {
                u32  rawtime = luaL_checkint(L, 1);
                bool aslocal = lua_toboolean(L, 2);
                luaL_argcheck(L, lua_isnumber(L, 2) || lua_isboolean(L, 2), 2, "expected number or boolean");

                timer::TimeInfo ti = timer::GetTimeInfo(rawtime, aslocal);

                luabridge::LuaRef t = luabridge::newTable(L);

                t["second"]  = ti.second;
                t["minute"]  = ti.minute;
                t["hour"]    = ti.hour;
                t["day"]     = ti.day;
                t["month"]   = ti.month;
                t["year"]    = ti.year;
                t["weekday"] = ti.weekday;
                t["yearday"] = ti.yearday;

                luabridge::push(L, t);

                return 1;
            }

            //---------------------------------------------------------
            bool RegisterTimerModule(lua_State* L)
            {
                luabridge::getGlobalNamespace(L)
                    .beginNamespace("timer")

                        .addCFunction("getTicks",    &timer_getTicks)
                        .addCFunction("getTime",     &timer_getTime)
                        .addCFunction("getTimeInfo", &timer_getTimeInfo)

                    .endNamespace();

                return true;
            }

        } // timer_module
    } // namespace script
} // namespace rpgss
