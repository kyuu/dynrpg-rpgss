#ifndef RPGSS_SCRIPT_IO_MODULE_CONSTANTS_HPP_INCLUDED
#define RPGSS_SCRIPT_IO_MODULE_CONSTANTS_HPP_INCLUDED

#include <string>


namespace rpgss {
    namespace script {
        namespace io_module {

            bool GetOpenModeConstant(const std::string& open_mode_str, int& out_open_mode);
            bool GetSeekModeConstant(const std::string& seek_mode_str, int& out_seek_mode);

        } // namespace io_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_IO_MODULE_CONSTANTS_HPP_INCLUDED
