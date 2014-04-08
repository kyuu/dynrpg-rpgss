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
