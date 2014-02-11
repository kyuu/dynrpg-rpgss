#ifndef RPGSS_ERROR_HPP_INCLUDED
#define RPGSS_ERROR_HPP_INCLUDED

#include <string>

#include "script/lua.hpp"


namespace rpgss {

    std::string FormatEventInfo(int eventId, int pageId, int lineId);
    void ReportError(const std::string& error_message, const std::string& event_info = "");
    void ReportLuaError(lua_State* L, const std::string& event_info = "");

} // namespace rpgss


#endif // RPGSS_ERROR_HPP_INCLUDED
