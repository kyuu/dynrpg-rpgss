/*
    The MIT License (MIT)

    Copyright (c) 2014 Anatoli Steinmark

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include <cstring>
#include <vector>
#include <string>

#include <DynRPG/DynRPG.h>

#include "common/types.hpp"
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
#define LUA_STATE rpgss::Context::Current().interpreter()


//---------------------------------------------------------
// Called when the plugin was loaded.
bool onStartup(char* pluginName)
{
    rpgss::debug::Log::Open(
        std::string("DynPlugins/") + (const char*)pluginName + ".log"
    );

    RPGSS_DEBUG_GUARD("onStartup()")

    rpgss::debug::Log() << "RPGSS version is " << RPGSS_VERSION;

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

    if (!rpgss::script::DoFile(LUA_STATE, "boot")) {
        rpgss::ReportLuaError(LUA_STATE);
        return;
    }

    lua_getglobal(LUA_STATE, "onInit");
    if (lua_isfunction(LUA_STATE, -1)) {
        if (!rpgss::script::Call(LUA_STATE, 0, 0)) {
            rpgss::ReportLuaError(LUA_STATE);
        }
    } else {
        lua_pop(LUA_STATE, 1); // pop result of lua_getglobal()
    }
}

//---------------------------------------------------------
// Called before the title screen is initialized.
void onInitTitleScreen()
{
    RPGSS_DEBUG_GUARD("onInitTitleScreen()")

    lua_getglobal(LUA_STATE, "onTitleScreen");
    if (lua_isfunction(LUA_STATE, -1)) {
        if (!rpgss::script::Call(LUA_STATE, 0, 0)) {
            rpgss::ReportLuaError(LUA_STATE);
        }
    } else {
        lua_pop(LUA_STATE, 1); // pop result of lua_getglobal()
    }
}

//---------------------------------------------------------
// Called before the player starts a new game.
void onNewGame()
{
    RPGSS_DEBUG_GUARD("onNewGame()")

    lua_getglobal(LUA_STATE, "onNewGame");
    if (lua_isfunction(LUA_STATE, -1)) {
        if (!rpgss::script::Call(LUA_STATE, 0, 0)) {
            rpgss::ReportLuaError(LUA_STATE);
        }
    } else {
        lua_pop(LUA_STATE, 1); // pop result of lua_getglobal()
    }
}

//---------------------------------------------------------
// Called before the player loads a game from a savestate.
void onLoadGame(int id, char* data, int length)
{
    RPGSS_DEBUG_GUARD("onLoadGame()")

    lua_getglobal(LUA_STATE, "onLoadGame");
    if (lua_isfunction(LUA_STATE, -1)) {
        // push function argument 1
        lua_pushinteger(LUA_STATE, id);

        // push function argument 2
        rpgss::core::ByteArray::Ptr saved_data = rpgss::core::ByteArray::New((const rpgss::u8*)data, length);
        rpgss::script::core_module::ByteArrayWrapper::Push(LUA_STATE, saved_data);

        // call function
        if (!rpgss::script::Call(LUA_STATE, 2, 0)) {
            rpgss::ReportLuaError(LUA_STATE);
        }
    } else {
        lua_pop(LUA_STATE, 1); // pop result of lua_getglobal()
    }
}

//---------------------------------------------------------
// Called before the player saves a game.
void onSaveGame(int id, void __cdecl (* savePluginData)(char* data, int length))
{
    RPGSS_DEBUG_GUARD("onSaveGame()")

    lua_getglobal(LUA_STATE, "onSaveGame");
    if (lua_isfunction(LUA_STATE, -1)) {
        // push function argument 1
        lua_pushinteger(LUA_STATE, id);

        // call function
        if (!rpgss::script::Call(LUA_STATE, 1, 1)) {
            rpgss::ReportLuaError(LUA_STATE);
        }

        // get data
        rpgss::core::ByteArray::Ptr data;
        if (!rpgss::script::core_module::ByteArrayWrapper::Is(LUA_STATE, -1)) {
            lua_pop(LUA_STATE, 1); // pop result of onSaveGame()
            rpgss::ReportError("onSaveGame() must return a byte array.");
            return;
        }
        data = rpgss::script::core_module::ByteArrayWrapper::Get(LUA_STATE, -1);

        // save data
        savePluginData((char*)data->getBuffer(), data->getSize());

        // pop result of onSaveGame()
        lua_pop(LUA_STATE, 1);
    } else {
        lua_pop(LUA_STATE, 1); // pop result of lua_getglobal()
    }
}

//---------------------------------------------------------
// Called every frame, before the screen is refreshed (see details!).
void onFrame(RPG::Scene scene)
{
    lua_getglobal(LUA_STATE, "onSceneDrawn");
    if (lua_isfunction(LUA_STATE, -1)) {
        // push function argument 1
        std::string scene_str;
        rpgss::script::game_module::GetSceneConstant(scene, scene_str);
        lua_pushlstring(LUA_STATE, scene_str.c_str(), scene_str.length());

        // call function
        if (!rpgss::script::Call(LUA_STATE, 1, 0)) {
            rpgss::ReportLuaError(LUA_STATE);
        }
    } else {
        lua_pop(LUA_STATE, 1); // pop result of lua_getglobal()
    }

    // allow Lua to perform a small incremental GC step
    // the overhead of this step should be negligible
    lua_gc(LUA_STATE, LUA_GCSTEP, 0);
}

//---------------------------------------------------------
// Called before an event or the hero is drawn.
bool onDrawEvent(RPG::Character* character, bool isHero)
{
    lua_getglobal(LUA_STATE, "onDrawCharacter");
    if (lua_isfunction(LUA_STATE, -1)) {
        // push function argument 1
        if (isHero) {
            rpgss::script::game_module::HeroWrapper::Push(LUA_STATE, (RPG::Hero*)character);
        } else {
            rpgss::script::game_module::EventWrapper::Push(LUA_STATE, (RPG::Event*)character);
        }

        // push function argument 2
        lua_pushboolean(LUA_STATE, isHero);

        // call function
        if (!rpgss::script::Call(LUA_STATE, 2, 1)) {
            rpgss::ReportLuaError(LUA_STATE);
            return true;
        }

        // get return value
        bool return_value;
        if (!lua_isboolean(LUA_STATE, -1)) {
            lua_pop(LUA_STATE, 1); // pop result of onDrawCharacter()
            rpgss::ReportError("onDrawCharacter() must return a boolean.");
            return true;
        }
        return_value = lua_toboolean(LUA_STATE, -1);

        // pop result of onDrawCharacter()
        lua_pop(LUA_STATE, 1);

        return return_value;
    } else {
        lua_pop(LUA_STATE, 1); // pop result of lua_getglobal()
    }

    return true;
}

//---------------------------------------------------------
// Called after an event or the hero was drawn (or was supposed to be drawn).
bool onEventDrawn(RPG::Character* character, bool isHero)
{
    lua_getglobal(LUA_STATE, "onCharacterDrawn");
    if (lua_isfunction(LUA_STATE, -1)) {
        // push function argument 1
        if (isHero) {
            rpgss::script::game_module::HeroWrapper::Push(LUA_STATE, (RPG::Hero*)character);
        } else {
            rpgss::script::game_module::EventWrapper::Push(LUA_STATE, (RPG::Event*)character);
        }

        // push function argument 2
        lua_pushboolean(LUA_STATE, isHero);

        // call function
        if (!rpgss::script::Call(LUA_STATE, 2, 0)) {
            rpgss::ReportLuaError(LUA_STATE);
            return true;
        }
    } else {
        lua_pop(LUA_STATE, 1); // pop result of lua_getglobal()
    }

    return true;
}

//---------------------------------------------------------
// Called before a battler is drawn.
bool onDrawBattler(RPG::Battler* battler, bool isMonster, int id)
{
    lua_getglobal(LUA_STATE, "onDrawBattler");
    if (lua_isfunction(LUA_STATE, -1)) {
        // push function argument 1
        if (isMonster) {
            rpgss::script::game_module::MonsterWrapper::Push(LUA_STATE, (RPG::Monster*)battler);
        } else {
            rpgss::script::game_module::ActorWrapper::Push(LUA_STATE, (RPG::Actor*)battler);
        }

        // push function argument 2
        lua_pushboolean(LUA_STATE, isMonster);

        // push function argument 3
        lua_pushinteger(LUA_STATE, id + 1 /* id is zero-based */);

        // call function
        if (!rpgss::script::Call(LUA_STATE, 3, 1)) {
            rpgss::ReportLuaError(LUA_STATE);
            return true;
        }

        // get return value
        bool return_value;
        if (!lua_isboolean(LUA_STATE, -1)) {
            lua_pop(LUA_STATE, 1); // pop result of onDrawBattler()
            rpgss::ReportError("onDrawBattler() must return a boolean.");
            return true;
        }
        return_value = lua_toboolean(LUA_STATE, -1);

        // pop result of onDrawBattler()
        lua_pop(LUA_STATE, 1);

        return return_value;
    } else {
        lua_pop(LUA_STATE, 1); // pop result of lua_getglobal()
    }

    return true;
}

//---------------------------------------------------------
// Called after a battler was drawn (or supposed to be drawn).
bool onBattlerDrawn(RPG::Battler* battler, bool isMonster, int id)
{
    lua_getglobal(LUA_STATE, "onBattlerDrawn");
    if (lua_isfunction(LUA_STATE, -1)) {
        // push function argument 1
        if (isMonster) {
            rpgss::script::game_module::MonsterWrapper::Push(LUA_STATE, (RPG::Monster*)battler);
        } else {
            rpgss::script::game_module::ActorWrapper::Push(LUA_STATE, (RPG::Actor*)battler);
        }

        // push function argument 2
        lua_pushboolean(LUA_STATE, isMonster);

        // push function argument 3
        lua_pushinteger(LUA_STATE, id + 1 /* id is zero-based */);

        // call function
        if (!rpgss::script::Call(LUA_STATE, 3, 0)) {
            rpgss::ReportLuaError(LUA_STATE);
            return true;
        }
    } else {
        lua_pop(LUA_STATE, 1); // pop result of lua_getglobal()
    }

    return true;
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
        lua_getglobal(LUA_STATE, func_name);
        if (lua_isfunction(LUA_STATE, -1) == 0) {
            lua_pop(LUA_STATE, lua_gettop(LUA_STATE)); // clean up stack
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
                    lua_pushnumber(LUA_STATE, parsedData->parameters[i].number);
                    break;
                }
                case RPG::PARAM_STRING: // easy!
                {
                    lua_pushstring(LUA_STATE, (const char*)parsedData->parameters[i].text);
                    break;
                }
                case RPG::PARAM_TOKEN: // still doable!
                {
                    rpgss::TokenParser tokenParser;
                    if (!tokenParser.parse((const char*)parsedData->parameters[i].text)) {
                        lua_pop(LUA_STATE, lua_gettop(LUA_STATE)); // clean up stack
                        std::stringstream error_message;
                        error_message << "bad argument #" << i << " to 'call' (token parse error)";
                        rpgss::ReportError(error_message.str(), rpgss::FormatEventInfo(eventId, pageId, lineId));
                        return true;
                    }
                    switch (tokenParser.ParsedType) {
                    case rpgss::TokenParser::ParsedBoolean:
                        lua_pushboolean(LUA_STATE, tokenParser.Boolean);
                        break;
                    case rpgss::TokenParser::ParsedNumber:
                        lua_pushnumber(LUA_STATE, tokenParser.Number);
                        break;
                    case rpgss::TokenParser::ParsedString:
                        lua_pushstring(LUA_STATE, tokenParser.String.c_str());
                        break;
                    case rpgss::TokenParser::ParsedColor:
                        lua_pushinteger(LUA_STATE, (rpgss::i32)tokenParser.Color);
                        break;
                    default:
                        break; // shut up compiler
                    }
                    break;
                }
                default: // shouldn't happen
                {
                    lua_pop(LUA_STATE, lua_gettop(LUA_STATE)); // clean up stack
                    std::stringstream error_message;
                    error_message << "bad argument #" << i << " to 'call' (unrecognized type)";
                    rpgss::ReportError(error_message.str(), rpgss::FormatEventInfo(eventId, pageId, lineId));
                    return true;
                }
            }
        }

        // call function
        if (!rpgss::script::Call(LUA_STATE, parsedData->parametersCount - 1, 0)) {
            rpgss::ReportLuaError(LUA_STATE, rpgss::FormatEventInfo(eventId, pageId, lineId));
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
