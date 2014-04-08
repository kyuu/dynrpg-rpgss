#ifndef RPGSS_SCRIPT_KEYBOARD_MODULE_CONSTANTS_HPP_INCLUDED
#define RPGSS_SCRIPT_KEYBOARD_MODULE_CONSTANTS_HPP_INCLUDED

#include <string>


namespace rpgss {
    namespace script {
        namespace keyboard_module {

            bool GetKeyConstant(int key, std::string& out_key_str);
            bool GetKeyConstant(const std::string& key_str, int& out_key);

        } // namespace keyboard_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_KEYBOARD_MODULE_CONSTANTS_HPP_INCLUDED
