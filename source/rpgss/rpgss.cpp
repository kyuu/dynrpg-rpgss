#include <cstring>
#include <vector>
#include <string>

#include <DynRPG/DynRPG.h>

#include "debug/debug.hpp"
#include "script/script.hpp"
#include "io/io.hpp"
#include "audio/audio.hpp"
#include "graphics/graphics.hpp"
#include "version.hpp"
#include "error.hpp"
#include "TokenParser.hpp"
#include "Context.hpp"

// for brevity
#define Interpreter() rpgss::Context::Current().interpreter()


//---------------------------------------------------------
// Called when the plugin was loaded.
bool onStartup(char* pluginName)
{
    rpgss::debug::Log::Open(
        std::string("DynPlugins/") + (const char*)pluginName + ".log"
    );

    RPGSS_DEBUG_GUARD("onStartup()")

    rpgss::debug::Log() << RPGSS_VERSION;

    rpgss::debug::Log() << "Initializing subsystems...";
    if (!rpgss::io::InitIoSubsystem() ||
        !rpgss::audio::InitAudioSubsystem() ||
        !rpgss::graphics::InitGraphicsSubsystem())
    {
        rpgss::debug::ShowError("Failed to initialize subsystems.");
        return false;
    }

    return true;
}

//---------------------------------------------------------
// Called after the RPG objects were initialized.
void onInitFinished()
{
    RPGSS_DEBUG_GUARD("onInitFinished()")

    if (!rpgss::Context::Open()) {
        rpgss::ReportError("Failed to open RPGSS context.");
        return;
    }

    if (!rpgss::script::DoFile(Interpreter(), "boot")) {
        rpgss::ReportLuaError(Interpreter());
        return;
    }

    lua_getglobal(Interpreter(), "onInit");
    if (lua_isfunction(Interpreter(), -1)) {
        if (!rpgss::script::Call(Interpreter(), 0, 0)) {
            rpgss::ReportLuaError(Interpreter());
        }
    } else {
        lua_pop(Interpreter(), 1); // pop result of lua_getglobal()
    }
}

//---------------------------------------------------------
// Called before the title screen is initialized.
void onInitTitleScreen()
{
    RPGSS_DEBUG_GUARD("onInitTitleScreen()")

    lua_getglobal(Interpreter(), "onTitleScreen");
    if (lua_isfunction(Interpreter(), -1)) {
        if (!rpgss::script::Call(Interpreter(), 0, 0)) {
            rpgss::ReportLuaError(Interpreter());
        }
    } else {
        lua_pop(Interpreter(), 1); // pop result of lua_getglobal()
    }
}

//---------------------------------------------------------
// Called before the player starts a new game.
void onNewGame()
{
    RPGSS_DEBUG_GUARD("onNewGame()")

    lua_getglobal(Interpreter(), "onNewGame");
    if (lua_isfunction(Interpreter(), -1)) {
        if (!rpgss::script::Call(Interpreter(), 0, 0)) {
            rpgss::ReportLuaError(Interpreter());
        }
    } else {
        lua_pop(Interpreter(), 1); // pop result of lua_getglobal()
    }
}

//---------------------------------------------------------
// Called before the player loads a game from a savestate.
void onLoadGame(int id, char* data, int length)
{
    RPGSS_DEBUG_GUARD("onLoadGame()")

    lua_getglobal(Interpreter(), "onLoadGame");
    if (lua_isfunction(Interpreter(), -1)) {
        if (!rpgss::script::Call(Interpreter(), 0, 0)) {
            rpgss::ReportLuaError(Interpreter());
        }
    } else {
        lua_pop(Interpreter(), 1); // pop result of lua_getglobal()
    }
}

//---------------------------------------------------------
// Called before the player saves a game.
void onSaveGame(int id, void __cdecl (* savePluginData)(char* data, int length))
{
    RPGSS_DEBUG_GUARD("onSaveGame()")

    lua_getglobal(Interpreter(), "onSaveGame");
    if (lua_isfunction(Interpreter(), -1)) {
        if (!rpgss::script::Call(Interpreter(), 0, 0)) {
            rpgss::ReportLuaError(Interpreter());
        }
    } else {
        lua_pop(Interpreter(), 1); // pop result of lua_getglobal()
    }
}

//---------------------------------------------------------
// Called every frame, before the screen is refreshed (see details!).
void onFrame(RPG::Scene scene)
{
    lua_getglobal(Interpreter(), "onFrame");
    if (lua_isfunction(Interpreter(), -1)) {
        if (!rpgss::script::Call(Interpreter(), 0, 0)) {
            rpgss::ReportLuaError(Interpreter());
        }
    } else {
        lua_pop(Interpreter(), 1); // pop result of lua_getglobal()
    }
}

//---------------------------------------------------------
// Called when a "Comment" event command is encountered.
bool onComment(const char* text, const RPG::ParsedCommentData* parsedData, RPG::EventScriptLine* nextScriptLine, RPG::EventScriptData* scriptData, int eventId, int pageId, int lineId, int* nextLineId)
{
    if (std::strcmp(parsedData->command, "call") == 0) {
        // we need at least the function name
        if (parsedData->parametersCount < 1) {
            rpgss::ReportError("wrong number of arguments to 'call' (expected at least one)", rpgss::FormatEventInfo(eventId, pageId, lineId));
            return true;
        }

        // get function name
        if (parsedData->parameters[0].type != RPG::PARAM_STRING && // allow: @call "func_name"
            parsedData->parameters[0].type != RPG::PARAM_TOKEN)    // allow: @call func_name
        {
            rpgss::ReportError("bad argument #1 to 'call' (expected a function name)", rpgss::FormatEventInfo(eventId, pageId, lineId));
            return true;
        }
        const char* func_name = (const char*)parsedData->parameters[0].text;

        // prepare function
        lua_getglobal(Interpreter(), func_name);
        if (lua_isfunction(Interpreter(), -1) == 0) {
            lua_pop(Interpreter(), lua_gettop(Interpreter())); // clean up stack
            std::stringstream error_message;
            error_message << "bad argument #1 to 'call' (global '" << func_name << "' not a function or does not exist)";
            rpgss::ReportError(error_message.str(), rpgss::FormatEventInfo(eventId, pageId, lineId));
            return true;
        }

        // prepare arguments, if any
        for (int i = 1; i < parsedData->parametersCount; i++) {
            switch (parsedData->parameters[i].type)
            {
                case RPG::PARAM_NUMBER: // easy!
                {
                    lua_pushnumber(Interpreter(), parsedData->parameters[i].number);
                    break;
                }
                case RPG::PARAM_STRING: // easy!
                {
                    lua_pushstring(Interpreter(), (const char*)parsedData->parameters[i].text);
                    break;
                }
                case RPG::PARAM_TOKEN: // still doable!
                {
                    rpgss::TokenParser tokenParser;
                    if (!tokenParser.parse((const char*)parsedData->parameters[i].text)) {
                        lua_pop(Interpreter(), lua_gettop(Interpreter())); // clean up stack
                        std::stringstream error_message;
                        error_message << "bad argument #" << i << " to 'call' (token parse error)";
                        rpgss::ReportError(error_message.str(), rpgss::FormatEventInfo(eventId, pageId, lineId));
                        return true;
                    }
                    switch (tokenParser.ParsedType) {
                    case rpgss::TokenParser::ParsedBoolean:
                        lua_pushboolean(Interpreter(), tokenParser.Boolean);
                        break;
                    case rpgss::TokenParser::ParsedNumber:
                        lua_pushnumber(Interpreter(), tokenParser.Number);
                        break;
                    case rpgss::TokenParser::ParsedString:
                        lua_pushstring(Interpreter(), tokenParser.String.c_str());
                        break;
                    case rpgss::TokenParser::ParsedColor:
                        lua_pushunsigned(Interpreter(), tokenParser.Color);
                        break;
                    default:
                        break; // shut up compiler
                    }
                    break;
                }
                default: // shouldn't happen
                {
                    lua_pop(Interpreter(), lua_gettop(Interpreter())); // clean up stack
                    std::stringstream error_message;
                    error_message << "bad argument #" << i << " to 'call' (unrecognized type)";
                    rpgss::ReportError(error_message.str(), rpgss::FormatEventInfo(eventId, pageId, lineId));
                    return true;
                }
            }
        }

        // call function
        if (!rpgss::script::Call(Interpreter(), parsedData->parametersCount - 1, 0)) {
            rpgss::ReportLuaError(Interpreter(), rpgss::FormatEventInfo(eventId, pageId, lineId));
            return true;
        }
    }

    return false;
}

//---------------------------------------------------------
// Called before the game exits.
void onExit()
{
    RPGSS_DEBUG_GUARD("onExit()")

    // destroy context
    rpgss::Context::Close();

    // shutdown subsystems
    rpgss::audio::DeinitAudioSubsystem();
    rpgss::io::DeinitIoSubsystem();
}
