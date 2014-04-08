#ifndef RPGSS_SCRIPT_CORE_MODULE_CORE_MODULE_HPP_INCLUDED
#define RPGSS_SCRIPT_CORE_MODULE_CORE_MODULE_HPP_INCLUDED

#include "../../core/ByteArray.hpp"
#include "../lua_include.hpp"
#include "ByteArrayWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace core_module {

            bool RegisterCoreModule(lua_State* L);

        } // core_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_CORE_MODULE_CORE_MODULE_HPP_INCLUDED
