#ifndef RPGSS_DEBUG_DEBUG_HPP_INCLUDED
#define RPGSS_DEBUG_DEBUG_HPP_INCLUDED

#include "Log.hpp"
#include "Guard.hpp"

#ifdef _DEBUG
#   define RPGSS_DEBUG_GUARD(block_name) rpgss::debug::Guard debug_guard_ ## __LINE__(block_name);
#else
#   define RPGSS_DEBUG_GUARD(block_name)
#endif


namespace rpgss {
    namespace debug {

        void ShowMessage(const char* format, ...);
        void ShowError(const char* format, ...);

    } // namespace debug
} // namespace rpgss


#endif // RPGSS_DEBUG_DEBUG_HPP_INCLUDED
