#ifndef RPGSS_SCRIPT_MOUSE_MODULE_MOUSE_MODULE_HPP_INCLUDED
#define RPGSS_SCRIPT_MOUSE_MODULE_MOUSE_MODULE_HPP_INCLUDED

#include "../../input/input.hpp"
#include "../lua_include.hpp"
#include "constants.hpp"


namespace rpgss {
    namespace script {
        namespace mouse_module {

            bool RegisterMouseModule(lua_State* L);

        } // mouse_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_MOUSE_MODULE_MOUSE_MODULE_HPP_INCLUDED
