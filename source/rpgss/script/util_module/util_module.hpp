#ifndef RPGSS_SCRIPT_UTIL_MODULE_UTIL_MODULE_HPP_INCLUDED
#define RPGSS_SCRIPT_UTIL_MODULE_UTIL_MODULE_HPP_INCLUDED

#include "../../util/util.hpp"
#include "../lua_include.hpp"


namespace rpgss {
    namespace script {
        namespace util_module {

            bool RegisterUtilModule(lua_State* L);

        } // util_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_UTIL_MODULE_UTIL_MODULE_HPP_INCLUDED
