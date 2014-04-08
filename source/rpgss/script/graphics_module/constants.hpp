#ifndef RPGSS_SCRIPT_GRAPHICS_MODULE_CONSTANTS_HPP_INCLUDED
#define RPGSS_SCRIPT_GRAPHICS_MODULE_CONSTANTS_HPP_INCLUDED

#include <string>


namespace rpgss {
    namespace script {
        namespace graphics_module {

            bool GetBlendModeConstant(int blend_mode, std::string& out_blend_mode_str);
            bool GetBlendModeConstant(const std::string& blend_mode_str, int& out_blend_mode);

        } // namespace graphics_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_GRAPHICS_MODULE_CONSTANTS_HPP_INCLUDED
