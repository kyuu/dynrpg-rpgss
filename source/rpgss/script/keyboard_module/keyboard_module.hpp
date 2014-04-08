#ifndef RPGSS_SCRIPT_KEYBOARD_MODULE_HPP_INCLUDED
#define RPGSS_SCRIPT_KEYBOARD_MODULE_HPP_INCLUDED

#include "../../input/input.hpp"
#include "../lua_include.hpp"
#include "constants.hpp"


namespace rpgss {
    namespace script {
        namespace keyboard_module {

            bool RegisterKeyboardModule(lua_State* L);

        } // keyboard_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_KEYBOARD_MODULE_HPP_INCLUDED
