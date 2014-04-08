#ifndef RPGSS_SCRIPT_MOUSE_MODULE_CONSTANTS_HPP_INCLUDED
#define RPGSS_SCRIPT_MOUSE_MODULE_CONSTANTS_HPP_INCLUDED

#include <string>


namespace rpgss {
    namespace script {
        namespace mouse_module {

            bool GetMouseButtonConstant(int mbutton, std::string& out_mbutton_str);
            bool GetMouseButtonConstant(const std::string& mbutton_str, int& out_mbutton);

        } // mouse_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_MOUSE_MODULE_CONSTANTS_HPP_INCLUDED
