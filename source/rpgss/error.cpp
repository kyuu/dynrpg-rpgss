#define NOT_MAIN_MODULE
#include <DynRPG/DynRPG.h>

#include <cstdlib>
#include <cassert>
#include <sstream>

#include "debug/debug.hpp"
#include "util/util.hpp"
#include "script/script.hpp"
#include "error.hpp"


namespace rpgss {

    namespace {

        //---------------------------------------------------------
        std::string FormatId(int id)
        {
            std::stringstream ss;
            if (id < 10) {
                ss << "000" << id;
            } else if (id < 100) {
                ss << "00" << id;
            } else if (id < 1000) {
                ss << "0" << id;
            } else {
                ss << id;
            }
            return ss.str();
        }

    } // anonymous namespace

    //---------------------------------------------------------
    std::string FormatEventInfo(int eventId, int pageId, int lineId)
    {
        std::stringstream ss;

        if (eventId < 0) {
            ss << "at CE" << FormatId(eventId * -1);
        } else if (eventId == 0) {
            ss << "at <unknown or battle event>";
        } else {
            ss << "at Map" << FormatId(RPG::hero->mapId) << ".lmu:EV" << FormatId(eventId);
        }

        ss << ":" << pageId << ":" << lineId << ":";

        return ss.str();
    }

    //---------------------------------------------------------
    void ReportError(const std::string& error_message, const std::string& event_info)
    {
        if (event_info.empty()) {
            debug::Log() << "Error:";
            debug::Log(1) << error_message;
            debug::ShowError(
                "Error:\n\n%s",
                error_message.c_str()
            );
        } else {
            debug::Log() << event_info;
            debug::Log(1) << "Error:";
            debug::Log(2) << error_message;
            debug::ShowError(
                "%s\n\nError:\n\n%s",
                event_info.c_str(),
                error_message.c_str()
            );
        }

        std::exit(1);
    }

    //---------------------------------------------------------
    void ReportLuaError(lua_State* L, const std::string& event_info)
    {
        assert(lua_gettop(L) == 1 && lua_isstring(L, 1));

        std::string error_message = lua_tostring(L, 1);
        std::string stack_traceback("stack traceback:\n\t(N/A)");

        if (error_message.find(RPGSS_STACK_TRACE_MAGIC) != std::string::npos) { // there is a stack trace appended
            std::vector<std::string> error_info = util::SplitString(error_message, RPGSS_STACK_TRACE_MAGIC);
            error_message = error_info[0];
            stack_traceback = error_info[1];
        }

        if (event_info.empty()) {
            debug::Log() << "Lua error:";
            debug::Log(1) << error_message;
            debug::Log() << stack_traceback;
            debug::ShowError("Lua error:\n\n%s", error_message.c_str());
        } else {
            debug::Log() << event_info;
            debug::Log(1) << "Lua error:";
            debug::Log(2) << error_message;
            debug::Log(1) << stack_traceback;
            debug::ShowError(
                "%s\n\nLua error:\n\n%s\n\nSee log file for more information.",
                event_info.c_str(),
                error_message.c_str()
            );
        }

        lua_pop(L, lua_gettop(L)); // clean up stack

        std::exit(1);
    }

} // namespace rpgss
