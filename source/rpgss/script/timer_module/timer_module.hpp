#ifndef RPGSS_SCRIPT_TIMER_MODULE_TIMER_MODULE_HPP_INCLUDED
#define RPGSS_SCRIPT_TIMER_MODULE_HPP_INCLUDED

#include "../../timer/timer.hpp"
#include "../lua_include.hpp"


namespace rpgss {
    namespace script {
        namespace timer_module {

            bool RegisterTimerModule(lua_State* L);

        } // timer_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_TIMER_MODULE_TIMER_MODULE_HPP_INCLUDED
