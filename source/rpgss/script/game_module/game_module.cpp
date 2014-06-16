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

#include <boost/unordered_map.hpp>
#include <boost/assign/list_of.hpp>

#define NOT_MAIN_MODULE
#include <DynRPG/DynRPG.h>

#include "../../Context.hpp"
#include "../../common/types.hpp"
#include "../core_module/core_module.hpp"
#include "../graphics_module/graphics_module.hpp"
#include "Screen.hpp"
#include "game_module.hpp"

#define RPGSS_SANE_SWITCH_ARRAY_SIZE_LIMIT   999999
#define RPGSS_SANE_VARIABLE_ARRAY_SIZE_LIMIT 999999


// Cherry's stuff not yet in his SDK
namespace RPG {

    //---------------------------------------------------------
    int getLowerLayerTileId(RPG::Map* map, int x, int y)
    {
        int ret;
        asm volatile("call *%%esi" : "=a" (ret), "=d" (RPG::_edx), "=c" (RPG::_ecx) : "S" (0x4A80CC), "a" (map), "d" (x), "c" (y) : "cc", "memory");
        return ret;
    }

    //---------------------------------------------------------
    int getUpperLayerTileId(RPG::Map* map, int x, int y)
    {
        int ret;
        asm volatile("call *%%esi" : "=a" (ret), "=d" (RPG::_edx), "=c" (RPG::_ecx) : "S" (0x4A80F4), "a" (map), "d" (x), "c" (y) : "cc", "memory");
        return ret;
    }

    //---------------------------------------------------------
    int getTerrainId(RPG::Map* map, int tileId)
    {
        int ret;
        asm volatile("movl 20(%%eax), %%eax; call *%%esi" : "=a" (ret), "=d" (RPG::_edx) : "S" (0x47D038), "a" (map), "d" (tileId) : "cc", "memory");
        return ret;
    }

    //---------------------------------------------------------
    int getMenuScreen()
    {
        return (**reinterpret_cast<char***>(0x4CDC60))[12];
    }

} // namespace RPG

namespace rpgss {
    namespace script {
        namespace game_module {

            /**********************************************************
             *                          MAP
             **********************************************************/

            //---------------------------------------------------------
            luabridge::LuaRef game_map_get_hero()
            {
                lua_State* L = Context::Current().interpreter();

                lua_pushstring(L, "__game_map_hero");
                lua_rawget(L, LUA_REGISTRYINDEX); // get hero from registry table

                if (!HeroWrapper::Is(L, -1)) {
                    HeroWrapper::Push(L, RPG::hero);

                    lua_pushstring(L, "__game_map_hero");
                    lua_pushvalue(L, -2); // push hero
                    lua_rawset(L, LUA_REGISTRYINDEX); // store hero in registry table

                    assert(HeroWrapper::Is(L, -1));
                }

                return luabridge::LuaRef::fromStack(L, -1);
            }

            //---------------------------------------------------------
            int game_map_events_proxy_mt_indexMetaMethod(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                if (n < 1 || n > RPG::map->events.count()) {
                   return luaL_error(L, "index out of bounds");
                }
                luabridge::push(L, EventWrapper(RPG::map->events.ptr->list[n-1]));
                return 1;
            }

            //---------------------------------------------------------
            int game_map_events_proxy_mt_newindexMetaMethod(lua_State* L)
            {
               return luaL_error(L, "operation not allowed");
            }

            //---------------------------------------------------------
            int game_map_events_proxy_mt_lenMetaMethod(lua_State* L)
            {
                lua_pushnumber(L, RPG::map->events.count());
                return 1;
            }

            //---------------------------------------------------------
            luabridge::LuaRef game_map_get_events()
            {
                lua_State* L = Context::Current().interpreter();

                lua_pushstring(L, "__game_map_events_proxy");
                lua_rawget(L, LUA_REGISTRYINDEX); // get proxy from registry table

                if (!lua_istable(L, -1)) { // proxy doesn't exist yet, create it
                    lua_newuserdata(L, 1 /* dummy size, we don't use the userdata memory */); // proxy

                    lua_newtable(L); // metatable

                    lua_pushstring(L, "__index");
                    lua_pushcfunction(L, game_map_events_proxy_mt_indexMetaMethod);
                    lua_rawset(L, -3); // add __index() to metatable

                    lua_pushstring(L, "__newindex");
                    lua_pushcfunction(L, game_map_events_proxy_mt_newindexMetaMethod);
                    lua_rawset(L, -3); // add __newindex() to metatable

                    lua_pushstring(L, "__len");
                    lua_pushcfunction(L, game_map_events_proxy_mt_lenMetaMethod);
                    lua_rawset(L, -3); // add __len() to metatable

                    lua_setmetatable(L, -2); // set metatable for proxy

                    lua_pushstring(L, "__game_map_events_proxy");
                    lua_pushvalue(L, -2); // push proxy
                    lua_rawset(L, LUA_REGISTRYINDEX); // store proxy in registry table

                    assert(lua_isuserdata(L, -1));
                }

                return luabridge::LuaRef::fromStack(L, -1);
            }

            //---------------------------------------------------------
            int game_map_get_id()
            {
                return RPG::hero->mapId;
            }

            //---------------------------------------------------------
            int game_map_get_width()
            {
                return RPG::map->getWidth();
            }

            //---------------------------------------------------------
            int game_map_get_height()
            {
                return RPG::map->getHeight();
            }

            //---------------------------------------------------------
            int game_map_get_cameraX()
            {
                return RPG::map->getCameraX();
            }

            //---------------------------------------------------------
            int game_map_get_cameraY()
            {
                return RPG::map->getCameraY();
            }

            //---------------------------------------------------------
            int game_map_existsEvent(lua_State* L)
            {
                if (lua_gettop(L) >= 1 && lua_type(L, 1) == LUA_TSTRING) {
                    // game.map.existsEvent(name)
                    const char* name = luaL_checkstring(L, 1);
                    for (int i = 0; i < RPG::map->events.count(); ++i) {
                        RPG::Event* event = RPG::map->events.ptr->list[i];
                        if (std::strcmp(event->getName().c_str(), name) == 0) {
                            lua_pushboolean(L, true);
                            return 1;
                        }
                    }
                    lua_pushboolean(L, false);
                } else {
                    // game.map.existsEvent(id)
                    int id = luaL_checkint(L, 1);
                    RPG::Event* event = RPG::map->events.get(id);
                    if (event) {
                        lua_pushboolean(L, true);
                    } else {
                        lua_pushboolean(L, false);
                    }
                }
                return 1;
            }

            //---------------------------------------------------------
            int game_map_findEvent(lua_State* L)
            {
                if (lua_gettop(L) >= 1 && lua_type(L, 1) == LUA_TSTRING) {
                    // game.map.findEvent(name)
                    const char* name = luaL_checkstring(L, 1);
                    for (int i = 0; i < RPG::map->events.count(); ++i) {
                        RPG::Event* event = RPG::map->events.ptr->list[i];
                        if (std::strcmp(event->getName().c_str(), name) == 0) {
                            luabridge::push(L, EventWrapper(event));
                            return 1;
                        }
                    }
                    lua_pushnil(L);
                } else {
                    // game.map.findEvent(id)
                    int id = luaL_checkint(L, 1);
                    RPG::Event* event = RPG::map->events.get(id);
                    if (event) {
                        luabridge::push(L, EventWrapper(event));
                    } else {
                        lua_pushnil(L);
                    }
                }
                return 1;
            }

            //---------------------------------------------------------
            int game_map_updateEvents(lua_State* L)
            {
                RPG::map->updateEvents();
                return 0;
            }

            //---------------------------------------------------------
            int game_map_getCameraPosition(lua_State* L)
            {
                lua_pushnumber(L, RPG::map->getCameraX());
                lua_pushnumber(L, RPG::map->getCameraY());
                return 2;
            }

            //---------------------------------------------------------
            int game_map_setCameraPosition(lua_State* L)
            {
                int x = luaL_checkint(L, 1);
                int y = luaL_checkint(L, 2);
                RPG::map->setCamera(x, y);
                return 0;
            }

            //---------------------------------------------------------
            int game_map_moveCamera(lua_State* L)
            {
                int ox = luaL_checkint(L, 1);
                int oy = luaL_checkint(L, 2);
                int speed = luaL_checkint(L, 3);
                RPG::map->moveCamera(ox, oy, speed);
                return 0;
            }

            //---------------------------------------------------------
            int game_map_getLowerLayerTileId(lua_State* L)
            {
                int x = luaL_checkint(L, 1);
                int y = luaL_checkint(L, 2);

                luaL_argcheck(L, x >= 0 && x < RPG::map->getWidth(), 1, "invalid x");
                luaL_argcheck(L, y >= 0 && y < RPG::map->getHeight(), 2, "invalid y");

                lua_pushnumber(L, RPG::getLowerLayerTileId(RPG::map, x, y));
                return 1;
            }

            //---------------------------------------------------------
            int game_map_getUpperLayerTileId(lua_State* L)
            {
                int x = luaL_checkint(L, 1);
                int y = luaL_checkint(L, 2);

                luaL_argcheck(L, x >= 0 && x < RPG::map->getWidth(), 1, "invalid x");
                luaL_argcheck(L, y >= 0 && y < RPG::map->getHeight(), 2, "invalid y");

                lua_pushnumber(L, RPG::getUpperLayerTileId(RPG::map, x, y));
                return 1;
            }

            //---------------------------------------------------------
            int game_map_getTerrainId(lua_State* L)
            {
                int tileId = luaL_checkint(L, 1);

                luaL_argcheck(L, tileId >= 0, 1, "invalid tile ID");

                lua_pushnumber(L, RPG::getTerrainId(RPG::map, tileId));
                return 1;
            }

            /**********************************************************
             *                        BATTLE
             **********************************************************/

            //---------------------------------------------------------
            int game_battle_enemies_proxy_mt_indexMetaMethod(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                if (n < 1 || n > RPG::monsters.count()) {
                   return luaL_error(L, "index out of bounds");
                }
                luabridge::push(L, MonsterWrapper(RPG::monsters[n-1]));
                return 1;
            }

            //---------------------------------------------------------
            int game_battle_enemies_proxy_mt_newindexMetaMethod(lua_State* L)
            {
               return luaL_error(L, "operation not allowed");
            }

            //---------------------------------------------------------
            int game_battle_enemies_proxy_mt_lenMetaMethod(lua_State* L)
            {
                lua_pushnumber(L, RPG::monsters.count());
                return 1;
            }

            //---------------------------------------------------------
            luabridge::LuaRef game_battle_get_enemies()
            {
                lua_State* L = Context::Current().interpreter();

                lua_pushstring(L, "__game_battle_enemies_proxy");
                lua_rawget(L, LUA_REGISTRYINDEX); // get proxy from registry table

                if (!lua_istable(L, -1)) { // proxy doesn't exist yet, create it
                    lua_newuserdata(L, 1 /* dummy size, we don't use the userdata memory */); // proxy

                    lua_newtable(L); // metatable

                    lua_pushstring(L, "__index");
                    lua_pushcfunction(L, game_battle_enemies_proxy_mt_indexMetaMethod);
                    lua_rawset(L, -3); // add __index() to metatable

                    lua_pushstring(L, "__newindex");
                    lua_pushcfunction(L, game_battle_enemies_proxy_mt_newindexMetaMethod);
                    lua_rawset(L, -3); // add __newindex() to metatable

                    lua_pushstring(L, "__len");
                    lua_pushcfunction(L, game_battle_enemies_proxy_mt_lenMetaMethod);
                    lua_rawset(L, -3); // add __len() to metatable

                    lua_setmetatable(L, -2); // set metatable for proxy

                    lua_pushstring(L, "__game_battle_enemies_proxy");
                    lua_pushvalue(L, -2); // push proxy
                    lua_rawset(L, LUA_REGISTRYINDEX); // store proxy in registry table

                    assert(lua_isuserdata(L, -1));
                }

                return luabridge::LuaRef::fromStack(L, -1);
            }

            /***********************************************************
             *                          INPUT
             **********************************************************/

            //---------------------------------------------------------
            int game_input_pressed(lua_State* L)
            {
                int nargs = lua_gettop(L);
                if (nargs < 1) {
                    luaL_error(L, "insufficient number of arguments (expected at least one)");
                    return 0;
                }

                bool result = false;
                for (int i = 1; i <= nargs; i++) {
                    const char* rpg_key_str = luaL_checkstring(L, i);
                    int rpg_key;
                    if (!GetRpgKeyConstant(rpg_key_str, rpg_key)) {
                        luaL_error(L, "bad argument #%d (invalid rpg key constant '%s')", i, rpg_key_str);
                        return 0;
                    }
                    result |= RPG::input->pressed((RPG::Key)rpg_key);
                }
                lua_pushboolean(L, result);

                return 1;
            }

            //---------------------------------------------------------
            int game_input_assign(lua_State* L)
            {
                const char* rpg_key_str = luaL_checkstring(L, 1);

                int rpg_key;
                if (!GetRpgKeyConstant(rpg_key_str, rpg_key)) {
                    const char* msg = lua_pushfstring(L, "invalid rpg key constant '%s'", rpg_key_str);
                    return luaL_argerror(L, 1, msg);
                }

                // make sure second parameter is a table
                if (lua_gettop(L) < 2) {
                    return luaL_argerror(L, 2, "expected table, got nothing");
                }
                if (!lua_istable(L, 2)) {
                    const char* msg = lua_pushfstring(L, "expected table, got %s", lua_typename(L, lua_type(L, 2)));
                    return luaL_argerror(L, 2, msg);
                }

                int* key_codes = RPG::input->key((RPG::Key)rpg_key);

                // create a table from current key codes
                lua_createtable(L, 8, 0);
                for (int i = 0; i < 8; i++) {
                    lua_pushnumber(L, key_codes[i]);
                    lua_rawseti(L, -2, i+1);
                }

                // assign new key codes
                for (int i = 0; i < 8; i++) {
                    lua_rawgeti(L, 2, i+1); // get number

                    if (!lua_isnumber(L, -1)) {
                        const char* msg = lua_pushfstring(L, "expected number at index %d, got %s", i+1, lua_typename(L, lua_type(L, -1)));
                        return luaL_argerror(L, 2, msg);
                    }

                    key_codes[i] = (int)lua_tonumber(L, -1);

                    lua_pop(L, 1); // pop number
                }

                // return previously created table
                return 1;
            }

            /***********************************************************
             *                         MUSIC
             **********************************************************/
/*
            //---------------------------------------------------------
            typedef RPG::Music* (RPG::System::*system_bgm_getter)();
            bool get_system_bgm_getter(const std::string& system_bgm_str, system_bgm_getter& out_get_system_bgm)
            {
                typedef boost::unordered_map<std::string, system_bgm_getter> map_type;

                static map_type map = boost::assign::map_list_of
                    ("title screen",     &RPG::System::getTitleBGM   )
                    ("battle",           &RPG::System::getBattleBGM  )
                    ("victory",          &RPG::System::getVictoryBGM )
                    ("inn",              &RPG::System::getInnBGM     )
                    ("skiff",            &RPG::System::getSkiffBGM   )
                    ("ship",             &RPG::System::getShipBGM    )
                    ("airship",          &RPG::System::getAirshipBGM )
                    ("game over screen", &RPG::System::getGameOverBGM);

                map_type::iterator mapped_value = map.find(system_bgm_str);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_get_system_bgm = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            int game_music_playSystemBGM(lua_State* L)
            {
                const char* system_bgm_str = luaL_checkstring(L, 1);

                system_bgm_getter get_system_bgm;
                if (!get_system_bgm_getter(system_bgm_str, get_system_bgm)) {
                    return luaL_error(L, "invalid system bgm constant");
                }
                (RPG::system->*get_system_bgm)()->play();

                return 0;
            }
*/
            //---------------------------------------------------------
            int game_music_play(lua_State* L)
            {
                const char* filename = luaL_checkstring(L, 1);
                int fade_in_time     = luaL_optint(L, 2, 0);
                int volume           = luaL_optint(L, 3, 100);
                int tempo            = luaL_optint(L, 4, 100);
                int pan              = luaL_optint(L, 5, 50);

                if (std::strlen(filename) == 0) {
                    return luaL_error(L, "empty filename");
                }
                if (fade_in_time < 0) {
                    return luaL_error(L, "invalid fade in time");
                }
                if (volume < 0 || volume > 100) {
                    return luaL_error(L, "invalid volume");
                }
                if (tempo < 50 || tempo > 150) {
                    return luaL_error(L, "invalid tempo");
                }
                if (pan < 0 || pan > 100) {
                    return luaL_error(L, "invalid pan");
                }

                RPG::Music bgm(filename, fade_in_time, volume, tempo, pan);
                bgm.play();

                return 0;
            }

            //---------------------------------------------------------
            int game_music_fadeOut(lua_State* L)
            {
                int fade_out_time = luaL_checkint(L, 1);
                RPG::Music::fadeOut(fade_out_time);
                return 0;
            }

            //---------------------------------------------------------
            int game_music_stop(lua_State* L)
            {
                RPG::Music::stop();
                return 0;
            }

            /***********************************************************
             *                         SOUND
             **********************************************************/
/*
            //---------------------------------------------------------
            typedef RPG::Sound* (RPG::System::*system_se_getter)();
            bool get_system_se_getter(const std::string& system_se_str, system_se_getter& out_get_system_se)
            {
                typedef boost::unordered_map<std::string, system_se_getter> map_type;

                static map_type map = boost::assign::map_list_of
                    ("cursor",            &RPG::System::getCursorSE     )
                    ("select choice",     &RPG::System::getDecisionSE   )
                    ("cancel",            &RPG::System::getCancelSE     )
                    ("buzzer",            &RPG::System::getBuzzerSE     )
                    ("battle transition", &RPG::System::getBattleStartSE)
                    ("flee battle",       &RPG::System::getFleeSE       )
                    ("enemy attacks",     &RPG::System::getEnemyAttackSE)
                    ("enemy damaged",     &RPG::System::getEnemyDamageSE)
                    ("ally damaged",      &RPG::System::getHeroDamageSE )
                    ("evade attack",      &RPG::System::getEvasionSE    )
                    ("enemy defeated",    &RPG::System::getEnemyDeathSE )
                    ("item used",         &RPG::System::getItemSE       );

                map_type::iterator mapped_value = map.find(system_se_str);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_get_system_se = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            int game_sound_playSystemSE(lua_State* L)
            {
                const char* system_se_str = luaL_checkstring(L, 1);

                system_se_getter get_system_se;
                if (!get_system_se_getter(system_se_str, get_system_se)) {
                    return luaL_error(L, "invalid system se constant");
                }
                (RPG::system->*get_system_se)()->play();

                return 0;
            }
*/
            //---------------------------------------------------------
            int game_sound_play(lua_State* L)
            {
                const char* filename = luaL_checkstring(L, 1);
                int volume           = luaL_optint(L, 2, 100);
                int tempo            = luaL_optint(L, 3, 100);
                int pan              = luaL_optint(L, 4, 50);

                if (std::strlen(filename) == 0) {
                    return luaL_error(L, "empty filename");
                }
                if (volume < 0 || volume > 100) {
                    return luaL_error(L, "invalid volume");
                }
                if (tempo < 50 || tempo > 150) {
                    return luaL_error(L, "invalid tempo");
                }
                if (pan < 0 || pan > 100) {
                    return luaL_error(L, "invalid pan");
                }

                RPG::Sound se(filename, volume, tempo, pan);
                se.play();

                return 0;
            }

            //---------------------------------------------------------
            int game_sound_stop(lua_State* L)
            {
                RPG::Sound::stop();
                return 0;
            }

            /***********************************************************
             *                        SCREEN
             **********************************************************/

            //---------------------------------------------------------
            int game_screen_get_width()
            {
                return RPG::screen->canvas->width();
            }

            //---------------------------------------------------------
            int game_screen_get_height()
            {
                return RPG::screen->canvas->height();
            }

            //---------------------------------------------------------
            int game_screen_get_brightness()
            {
                return RPG::screen->canvas->brightness;
            }

            //---------------------------------------------------------
            void game_screen_set_brightness(int brightness)
            {
                RPG::screen->canvas->brightness = brightness;
            }

            //---------------------------------------------------------
            int game_screen_getDimensions(lua_State* L)
            {
                lua_pushnumber(L, RPG::screen->canvas->width());
                lua_pushnumber(L, RPG::screen->canvas->height());
                return 2;
            }

            //---------------------------------------------------------
            int game_screen_getClipRect(lua_State* L)
            {
                const core::Recti& clip_rect = Screen::GetClipRect();
                lua_pushnumber(L, clip_rect.getX());
                lua_pushnumber(L, clip_rect.getY());
                lua_pushnumber(L, clip_rect.getWidth());
                lua_pushnumber(L, clip_rect.getHeight());
                return 4;
            }

            //---------------------------------------------------------
            int game_screen_setClipRect(lua_State* L)
            {
                int x = luaL_checkint(L, 1);
                int y = luaL_checkint(L, 2);
                int w = luaL_checkint(L, 3);
                int h = luaL_checkint(L, 4);

                core::Recti clip_rect = core::Recti(x, y, w, h);
                core::Recti screen_bounds = core::Recti(Screen::GetWidth(), Screen::GetHeight());

                if (!clip_rect.isValid() || !clip_rect.isInside(screen_bounds)) {
                    return luaL_error(L, "invalid rect");
                }

                Screen::SetClipRect(clip_rect);
                return 0;
            }

            //---------------------------------------------------------
            int game_screen_copyRect(lua_State* L)
            {
                int x = luaL_checkint(L, 1);
                int y = luaL_checkint(L, 2);
                int w = luaL_checkint(L, 3);
                int h = luaL_checkint(L, 4);
                graphics::Image* destination = graphics_module::ImageWrapper::GetOpt(L, 5);

                core::Recti rect = core::Recti(x, y, w, h);
                core::Recti screen_bounds = core::Recti(Screen::GetWidth(), Screen::GetHeight());

                if (!rect.isValid() || !rect.isInside(screen_bounds)) {
                    return luaL_error(L, "invalid rect");
                }

                graphics::Image::Ptr result = Screen::CopyRect(rect, destination);

                if (result) {
                    graphics_module::ImageWrapper::Push(L, result);
                } else {
                    lua_pushnil(L);
                }

                return 1;
            }

            //---------------------------------------------------------
            int game_screen_clear(lua_State* L)
            {
                u32 color = (u32)luaL_optint(L, 1, 0x000000FF);
                Screen::Clear(graphics::RGBA8888ToRGBA(color));
                return 0;
            }

            //---------------------------------------------------------
            int game_screen_grey(lua_State* L)
            {
                Screen::Grey();
                return 0;
            }

            //---------------------------------------------------------
            int game_screen_getPixel(lua_State* L)
            {
                int x = luaL_checkint(L, 1);
                int y = luaL_checkint(L, 2);

                luaL_argcheck(L, x >= 0 && x < Screen::GetWidth(),  1, "invalid x");
                luaL_argcheck(L, y >= 0 && y < Screen::GetHeight(), 2, "invalid y");

                u16 c = Screen::GetPixel(x, y);
                lua_pushinteger(L, (i32)graphics::RGB565ToRGBA8888(c));

                return 1;
            }

            //---------------------------------------------------------
            int game_screen_setPixel(lua_State* L)
            {
                int x = luaL_checkint(L, 1);
                int y = luaL_checkint(L, 2);
                u32 c = (u32)luaL_checkint(L, 3);

                luaL_argcheck(L, x >= 0 && x < Screen::GetWidth(),  1, "invalid x");
                luaL_argcheck(L, y >= 0 && y < Screen::GetHeight(), 2, "invalid y");

                Screen::SetPixel(x, y, graphics::RGBA8888ToRGB565(c));

                return 0;
            }

            //---------------------------------------------------------
            int game_screen_drawPoint(lua_State* L)
            {
                int x = luaL_checkint(L, 1);
                int y = luaL_checkint(L, 2);
                u32 c = (u32)luaL_checkint(L, 3);

                int blend_mode;
                const char* blend_mode_str = luaL_optstring(L, 4, "mix");
                if (!graphics_module::GetBlendModeConstant(blend_mode_str, blend_mode)) {
                    return luaL_argerror(L, 4, "invalid blend mode constant");
                }

                Screen::DrawPoint(
                    core::Vec2i(x, y),
                    graphics::RGBA8888ToRGBA(c),
                    blend_mode
                );

                return 0;
            }

            //---------------------------------------------------------
            int game_screen_drawLine(lua_State* L)
            {
                int x1 = luaL_checkint(L, 1);
                int y1 = luaL_checkint(L, 2);
                int x2 = luaL_checkint(L, 3);
                int y2 = luaL_checkint(L, 4);
                u32 c1 = luaL_checkint(L, 5);
                u32 c2 = luaL_optint(L, 6, c1);

                int blend_mode;
                const char* blend_mode_str = luaL_optstring(L, 7, "mix");
                if (!graphics_module::GetBlendModeConstant(blend_mode_str, blend_mode)) {
                    return luaL_argerror(L, 7, "invalid blend mode constant");
                }

                Screen::DrawLine(
                    core::Vec2i(x1, y1),
                    core::Vec2i(x2, y2),
                    graphics::RGBA8888ToRGBA(c1),
                    graphics::RGBA8888ToRGBA(c2),
                    blend_mode
                );

                return 0;
            }

            //---------------------------------------------------------
            int game_screen_drawRectangle(lua_State* L)
            {
                bool fill = lua_toboolean(L, 1);
                int     x = luaL_checkint(L, 2);
                int     y = luaL_checkint(L, 3);
                int     w = luaL_checkint(L, 4);
                int     h = luaL_checkint(L, 5);
                u32    c1 = luaL_checkint(L, 6);
                u32    c2 = luaL_optint(L, 7, c1);
                u32    c3 = luaL_optint(L, 8, c1);
                u32    c4 = luaL_optint(L, 9, c1);

                int blend_mode;
                const char* blend_mode_str = luaL_optstring(L, 10, "mix");
                if (!graphics_module::GetBlendModeConstant(blend_mode_str, blend_mode)) {
                    return luaL_argerror(L, 10, "invalid blend mode constant");
                }

                Screen::DrawRectangle(
                    fill,
                    core::Recti(x, y, w, h),
                    graphics::RGBA8888ToRGBA(c1),
                    graphics::RGBA8888ToRGBA(c2),
                    graphics::RGBA8888ToRGBA(c3),
                    graphics::RGBA8888ToRGBA(c4),
                    blend_mode
                );

                return 0;
            }

            //---------------------------------------------------------
            int game_screen_drawCircle(lua_State* L)
            {
                bool fill = lua_toboolean(L, 1);
                int     x = luaL_checkint(L, 2);
                int     y = luaL_checkint(L, 3);
                int     r = luaL_checkint(L, 4);
                u32    c1 = luaL_checkint(L, 5);
                u32    c2 = luaL_optint(L, 6, c1);

                int blend_mode;
                const char* blend_mode_str = luaL_optstring(L, 7, "mix");
                if (!graphics_module::GetBlendModeConstant(blend_mode_str, blend_mode)) {
                    return luaL_argerror(L, 7, "invalid blend mode constant");
                }

                Screen::DrawCircle(
                    fill,
                    core::Vec2i(x, y),
                    r,
                    graphics::RGBA8888ToRGBA(c1),
                    graphics::RGBA8888ToRGBA(c2),
                    blend_mode
                );

                return 0;
            }

            //---------------------------------------------------------
            int game_screen_drawTriangle(lua_State* L)
            {
                /*
                bool fill = lua_toboolean(L, 1);
                int    x1 = luaL_checkint(L, 2);
                int    y1 = luaL_checkint(L, 3);
                int    x2 = luaL_checkint(L, 4);
                int    y2 = luaL_checkint(L, 5);
                int    x3 = luaL_checkint(L, 6);
                int    y3 = luaL_checkint(L, 7);
                u32    c1 = luaL_checkunsigned(L, 8);
                u32    c2 = luaL_optunsigned(L, 9, c1);
                u32    c3 = luaL_optunsigned(L, 10, c1);

                int blend_mode;
                const char* blend_mode_str = luaL_optstring(L, 11, "mix");
                if (!graphics_module::GetBlendModeConstant(blend_mode_str, blend_mode)) {
                    return luaL_argerror(L, 11, "invalid blend mode constant");
                }
                */

                // TODO

                return 0;
            }

            //---------------------------------------------------------
            int game_screen_draw(lua_State* L)
            {
                graphics::Image* that = 0;
                int   sx, sy, sw, sh;
                int   x, y;
                float angle;
                float scale;
                u32   color;
                int blend_mode;

                int nargs = lua_gettop(L);
                if (nargs >= 7 && lua_type(L, 7) == LUA_TNUMBER)
                {
                    that = graphics_module::ImageWrapper::Get(L, 1);
                    sx   = luaL_checkint(L, 2);
                    sy   = luaL_checkint(L, 3);
                    sw   = luaL_checkint(L, 4);
                    sh   = luaL_checkint(L, 5);
                    x    = luaL_checkint(L, 6);
                    y    = luaL_checkint(L, 7);
                    angle = luaL_optnumber(L, 8, 0.0);
                    scale = luaL_optnumber(L, 9, 1.0);
                    color = luaL_optint(L, 10, 0xFFFFFFFF);

                    const char* blend_mode_str = luaL_optstring(L, 11, "mix");
                    if (!graphics_module::GetBlendModeConstant(blend_mode_str, blend_mode)) {
                        return luaL_argerror(L, 11, "invalid blend mode constant");
                    }
                }
                else
                {
                    that = graphics_module::ImageWrapper::Get(L, 1);
                    x    = luaL_checkint(L, 2);
                    y    = luaL_checkint(L, 3);
                    angle = luaL_optnumber(L, 4, 0.0);
                    scale = luaL_optnumber(L, 5, 1.0);
                    color = luaL_optint(L, 6, 0xFFFFFFFF);

                    const char* blend_mode_str = luaL_optstring(L, 7, "mix");
                    if (!graphics_module::GetBlendModeConstant(blend_mode_str, blend_mode)) {
                        return luaL_argerror(L, 7, "invalid blend mode constant");
                    }

                    sx = 0;
                    sy = 0;
                    sw = that->getWidth();
                    sh = that->getHeight();
                }

                Screen::Draw(
                    that,
                    core::Recti(sx, sy, sw, sh),
                    core::Vec2i(x, y),
                    angle,
                    scale,
                    graphics::RGBA8888ToRGBA(color),
                    blend_mode
                );

                return 0;
            }

            //---------------------------------------------------------
            int game_screen_drawq(lua_State* L)
            {
                graphics::Image* that = 0;
                int sx, sy, sw, sh;
                int x1, y1;
                int x2, y2;
                int x3, y3;
                int x4, y4;
                u32 color;
                int blend_mode;

                int nargs = lua_gettop(L);
                if (nargs >= 13)
                {
                    that = graphics_module::ImageWrapper::Get(L, 1);
                    sx   = luaL_checkint(L, 2);
                    sy   = luaL_checkint(L, 3);
                    sw   = luaL_checkint(L, 4);
                    sh   = luaL_checkint(L, 5);
                    x1   = luaL_checkint(L, 6);
                    y1   = luaL_checkint(L, 7);
                    x2   = luaL_checkint(L, 8);
                    y2   = luaL_checkint(L, 9);
                    x3   = luaL_checkint(L, 10);
                    y3   = luaL_checkint(L, 11);
                    x4   = luaL_checkint(L, 12);
                    y4   = luaL_checkint(L, 13);
                    color = luaL_optint(L, 14, 0xFFFFFFFF);

                    const char* blend_mode_str = luaL_optstring(L, 15, "mix");
                    if (!graphics_module::GetBlendModeConstant(blend_mode_str, blend_mode)) {
                        return luaL_argerror(L, 15, "invalid blend mode constant");
                    }
                }
                else
                {
                    that = graphics_module::ImageWrapper::Get(L, 1);
                    x1   = luaL_checkint(L, 2);
                    y1   = luaL_checkint(L, 3);
                    x2   = luaL_checkint(L, 4);
                    y2   = luaL_checkint(L, 5);
                    x3   = luaL_checkint(L, 6);
                    y3   = luaL_checkint(L, 7);
                    x4   = luaL_checkint(L, 8);
                    y4   = luaL_checkint(L, 9);
                    color = luaL_optint(L, 10, 0xFFFFFFFF);

                    const char* blend_mode_str = luaL_optstring(L, 11, "mix");
                    if (!graphics_module::GetBlendModeConstant(blend_mode_str, blend_mode)) {
                        return luaL_argerror(L, 11, "invalid blend mode constant");
                    }

                    sx = 0;
                    sy = 0;
                    sw = that->getWidth();
                    sh = that->getHeight();
                }

                Screen::Drawq(
                    that,
                    core::Recti(sx, sy, sw, sh),
                    core::Vec2i(x1, y1),
                    core::Vec2i(x2, y2),
                    core::Vec2i(x3, y3),
                    core::Vec2i(x4, y4),
                    graphics::RGBA8888ToRGBA(color),
                    blend_mode
                );

                return 0;
            }

            //---------------------------------------------------------
            int game_screen_drawText(lua_State* L)
            {
                graphics::Font* font = graphics_module::FontWrapper::Get(L, 1);
                int x                = luaL_checkint(L, 2);
                int y                = luaL_checkint(L, 3);
                size_t len;
                const char* text     = luaL_checklstring(L, 4, &len);
                float scale          = luaL_optnumber(L, 5, 1.0);
                u32 color            = luaL_optint(L, 6, 0xFFFFFFFF);

                Screen::DrawText(
                    font,
                    core::Vec2i(x, y),
                    text,
                    len,
                    scale,
                    graphics::RGBA8888ToRGBA(color)
                );

                return 0;
            }

            //---------------------------------------------------------
            int game_screen_drawWindow(lua_State* L)
            {
                graphics::WindowSkin* windowSkin = graphics_module::WindowSkinWrapper::Get(L, 1);
                int x = luaL_checkint(L, 2);
                int y = luaL_checkint(L, 3);
                int w = luaL_checkint(L, 4);
                int h = luaL_checkint(L, 5);

                luaL_argcheck(L, w >= 0, 4, "invalid width");
                luaL_argcheck(L, h >= 0, 5, "invalid height");

                Screen::DrawWindow(
                    windowSkin,
                    core::Recti(x, y, w, h)
                );

                return 0;
            }

            /***********************************************************
             *                         GAME
             **********************************************************/

            //---------------------------------------------------------
            std::string game_get_scene()
            {
                std::string scene_str;
                if (!GetSceneConstant(RPG::system->scene, scene_str)) {
                    luaL_error(Context::Current().interpreter(), "unexpected internal value");
                }
                return scene_str;
            }

            //---------------------------------------------------------
            bool game_get_isTestPlay()
            {
                return RPG::isTestPlay;
            }

            //---------------------------------------------------------
            bool game_get_isBattleTest()
            {
                return RPG::isBattleTest;
            }

            //---------------------------------------------------------
            int game_get_saveCount()
            {
                return RPG::system->saveCount;
            }

            //---------------------------------------------------------
            int game_variables_proxy_mt_indexMetaMethod(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                luaL_argcheck(L, n >= 1 && n <= RPGSS_SANE_VARIABLE_ARRAY_SIZE_LIMIT, 2, "index out of bounds");
                if (n > RPG::system->variables.size) {
                    RPG::system->variables.resize(n);
                }
                lua_pushnumber(L, RPG::variables[n]);
                return 1;
            }

            //---------------------------------------------------------
            int game_variables_proxy_mt_newindexMetaMethod(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                int v = luaL_checkint(L, 3);
                luaL_argcheck(L, n >= 1 && n <= RPGSS_SANE_VARIABLE_ARRAY_SIZE_LIMIT, 2, "index out of bounds");
                if (n > RPG::system->variables.size) {
                    RPG::system->variables.resize(n);
                }
                RPG::variables[n] = v;
                return 0;
            }

            //---------------------------------------------------------
            int game_variables_proxy_mt_lenMetaMethod(lua_State* L)
            {
                lua_pushnumber(L, RPG::system->variables.size);
                return 1;
            }

            //---------------------------------------------------------
            luabridge::LuaRef game_get_variables()
            {
                lua_State* L = Context::Current().interpreter();

                lua_pushstring(L, "__game_variables_proxy");
                lua_rawget(L, LUA_REGISTRYINDEX); // get proxy from registry table

                if (!lua_istable(L, -1)) { // proxy doesn't exist yet, create it
                    lua_newuserdata(L, 1 /* dummy size, we don't use the userdata memory */); // proxy

                    lua_newtable(L); // metatable

                    lua_pushstring(L, "__index");
                    lua_pushcfunction(L, game_variables_proxy_mt_indexMetaMethod);
                    lua_rawset(L, -3); // add __index() to metatable

                    lua_pushstring(L, "__newindex");
                    lua_pushcfunction(L, game_variables_proxy_mt_newindexMetaMethod);
                    lua_rawset(L, -3); // add __newindex() to metatable

                    lua_pushstring(L, "__len");
                    lua_pushcfunction(L, game_variables_proxy_mt_lenMetaMethod);
                    lua_rawset(L, -3); // add __len() to metatable

                    lua_setmetatable(L, -2); // set metatable for proxy

                    lua_pushstring(L, "__game_variables_proxy");
                    lua_pushvalue(L, -2); // push proxy
                    lua_rawset(L, LUA_REGISTRYINDEX); // store proxy in registry table

                    assert(lua_isuserdata(L, -1));
                }

                return luabridge::LuaRef::fromStack(L, -1);
            }

            //---------------------------------------------------------
            int game_switches_proxy_mt_indexMetaMethod(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                luaL_argcheck(L, n >= 1 && n <= RPGSS_SANE_SWITCH_ARRAY_SIZE_LIMIT, 2, "index out of bounds");
                if (n > RPG::system->switches.size) {
                    RPG::system->switches.resize(n);
                }
                lua_pushboolean(L, RPG::switches[n]);
                return 1;
            }

            //---------------------------------------------------------
            int game_switches_proxy_mt_newindexMetaMethod(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                bool v = lua_toboolean(L, 3);
                luaL_argcheck(L, n >= 1 && n <= RPGSS_SANE_SWITCH_ARRAY_SIZE_LIMIT, 2, "index out of bounds");
                luaL_argcheck(L, lua_isboolean(L, 3) || lua_isnumber(L, 3), 3, "expected number or boolean");
                if (n > RPG::system->switches.size) {
                    RPG::system->switches.resize(n);
                }
                RPG::switches[n] = v;
                return 0;
            }

            //---------------------------------------------------------
            int game_switches_proxy_mt_lenMetaMethod(lua_State* L)
            {
                lua_pushnumber(L, RPG::system->switches.size);
                return 1;
            }

            //---------------------------------------------------------
            luabridge::LuaRef game_get_switches()
            {
                lua_State* L = Context::Current().interpreter();

                lua_pushstring(L, "__game_switches_proxy");
                lua_rawget(L, LUA_REGISTRYINDEX); // get proxy from registry table

                if (!lua_istable(L, -1)) { // proxy doesn't exist yet, create it
                    lua_newuserdata(L, 1 /* dummy size, we don't use the userdata memory */); // proxy

                    lua_newtable(L); // metatable

                    lua_pushstring(L, "__index");
                    lua_pushcfunction(L, game_switches_proxy_mt_indexMetaMethod);
                    lua_rawset(L, -3); // add __index() to metatable

                    lua_pushstring(L, "__newindex");
                    lua_pushcfunction(L, game_switches_proxy_mt_newindexMetaMethod);
                    lua_rawset(L, -3); // add __newindex() to metatable

                    lua_pushstring(L, "__len");
                    lua_pushcfunction(L, game_switches_proxy_mt_lenMetaMethod);
                    lua_rawset(L, -3); // add __len() to metatable

                    lua_setmetatable(L, -2); // set metatable for proxy

                    lua_pushstring(L, "__game_switches_proxy");
                    lua_pushvalue(L, -2); // push proxy
                    lua_rawset(L, LUA_REGISTRYINDEX); // store proxy in registry table

                    assert(lua_isuserdata(L, -1));
                }

                return luabridge::LuaRef::fromStack(L, -1);
            }

            //---------------------------------------------------------
            int game_actors_proxy_mt_indexMetaMethod(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                if (n < 1 || n > RPG::actors.count()) {
                   return luaL_error(L, "index out of bounds");
                }
                luabridge::push(L, ActorWrapper(RPG::actors[n]));
                return 1;
            }

            //---------------------------------------------------------
            int game_actors_proxy_mt_newindexMetaMethod(lua_State* L)
            {
               return luaL_error(L, "operation not allowed");
            }

            //---------------------------------------------------------
            int game_actors_proxy_mt_lenMetaMethod(lua_State* L)
            {
                lua_pushnumber(L, RPG::actors.count());
                return 1;
            }

            //---------------------------------------------------------
            luabridge::LuaRef game_get_actors()
            {
                lua_State* L = Context::Current().interpreter();

                lua_pushstring(L, "__game_actors_proxy");
                lua_rawget(L, LUA_REGISTRYINDEX); // get proxy from registry table

                if (!lua_istable(L, -1)) { // proxy doesn't exist yet, create it
                    lua_newuserdata(L, 1 /* dummy size, we don't use the userdata memory */); // proxy

                    lua_newtable(L); // metatable

                    lua_pushstring(L, "__index");
                    lua_pushcfunction(L, game_actors_proxy_mt_indexMetaMethod);
                    lua_rawset(L, -3); // add __index() to metatable

                    lua_pushstring(L, "__newindex");
                    lua_pushcfunction(L, game_actors_proxy_mt_newindexMetaMethod);
                    lua_rawset(L, -3); // add __newindex() to metatable

                    lua_pushstring(L, "__len");
                    lua_pushcfunction(L, game_actors_proxy_mt_lenMetaMethod);
                    lua_rawset(L, -3); // add __len() to metatable

                    lua_setmetatable(L, -2); // set metatable for proxy

                    lua_pushstring(L, "__game_actors_proxy");
                    lua_pushvalue(L, -2); // push proxy
                    lua_rawset(L, LUA_REGISTRYINDEX); // store proxy in registry table

                    assert(lua_isuserdata(L, -1));
                }

                return luabridge::LuaRef::fromStack(L, -1);
            }

            //---------------------------------------------------------
            int game_get_actor_party_size()
            {
                int party_size = 0;
                for (int i = 0; i < 4; i++) {
                    if (RPG::Actor::partyMember(i) == 0) {
                        break;
                    }
                    party_size++;
                }
                return party_size;
            }

            //---------------------------------------------------------
            int game_party_proxy_mt_indexMetaMethod(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                if (n < 1 || n > game_get_actor_party_size()) {
                   return luaL_error(L, "index out of bounds");
                }
                luabridge::push(L, ActorWrapper(RPG::Actor::partyMember(n-1)));
                return 1;
            }

            //---------------------------------------------------------
            int game_party_proxy_mt_newindexMetaMethod(lua_State* L)
            {
               return luaL_error(L, "operation not allowed");
            }

            //---------------------------------------------------------
            int game_party_proxy_mt_lenMetaMethod(lua_State* L)
            {
                lua_pushnumber(L, game_get_actor_party_size());
                return 1;
            }

            //---------------------------------------------------------
            luabridge::LuaRef game_get_party()
            {
                lua_State* L = Context::Current().interpreter();

                lua_pushstring(L, "__game_party_proxy");
                lua_rawget(L, LUA_REGISTRYINDEX); // get proxy from registry table

                if (!lua_istable(L, -1)) { // proxy doesn't exist yet, create it
                    lua_newuserdata(L, 1 /* dummy size, we don't use the userdata memory */); // proxy

                    lua_newtable(L); // metatable

                    lua_pushstring(L, "__index");
                    lua_pushcfunction(L, game_party_proxy_mt_indexMetaMethod);
                    lua_rawset(L, -3); // add __index() to metatable

                    lua_pushstring(L, "__newindex");
                    lua_pushcfunction(L, game_party_proxy_mt_newindexMetaMethod);
                    lua_rawset(L, -3); // add __newindex() to metatable

                    lua_pushstring(L, "__len");
                    lua_pushcfunction(L, game_party_proxy_mt_lenMetaMethod);
                    lua_rawset(L, -3); // add __len() to metatable

                    lua_setmetatable(L, -2); // set metatable for proxy

                    lua_pushstring(L, "__game_party_proxy");
                    lua_pushvalue(L, -2); // push proxy
                    lua_rawset(L, LUA_REGISTRYINDEX); // store proxy in registry table

                    assert(lua_isuserdata(L, -1));
                }

                return luabridge::LuaRef::fromStack(L, -1);
            }

            //---------------------------------------------------------
            int game_get_fps()
            {
                return RPG::screen->maxFPS;
            }

            //---------------------------------------------------------
            void game_set_fps(int fps)
            {
                return RPG::screen->setFPS(fps);
            }

            //---------------------------------------------------------
            double game_get_millisPerFrame()
            {
                return RPG::screen->millisecondsPerFrame;
            }

            //---------------------------------------------------------
            int game_get_frameCounter()
            {
                return RPG::system->frameCounter;
            }

            //---------------------------------------------------------
            std::string game_menu_get_scene()
            {
                std::string menuscene_str;
                if (!GetMenuSceneConstant(RPG::getMenuScreen(), menuscene_str)) {
                    luaL_error(Context::Current().interpreter(), "unexpected internal value");
                }
                return menuscene_str;
            }

            //---------------------------------------------------------
            bool game_menu_get_allowed()
            {
                return RPG::system->menuAllowed;
            }

            //---------------------------------------------------------
            void game_menu_set_allowed(bool allowed)
            {
                RPG::system->menuAllowed = allowed;
            }

            //---------------------------------------------------------
            bool game_menu_get_saveAllowed()
            {
                return RPG::system->saveAllowed;
            }

            //---------------------------------------------------------
            void game_menu_set_saveAllowed(bool saveAllowed)
            {
                RPG::system->saveAllowed = saveAllowed;
            }

            //---------------------------------------------------------
            bool game_menu_get_teleportAllowed()
            {
                return RPG::system->teleportAllowed;
            }

            //---------------------------------------------------------
            void game_menu_set_teleportAllowed(bool teleportAllowed)
            {
                RPG::system->teleportAllowed = teleportAllowed;
            }

            //---------------------------------------------------------
            bool game_menu_get_escapeAllowed()
            {
                return RPG::system->escapeAllowed;
            }

            //---------------------------------------------------------
            void game_menu_set_escapeAllowed(bool escapeAllowed)
            {
                RPG::system->escapeAllowed = escapeAllowed;
            }

            //---------------------------------------------------------
            bool RegisterGameModule(lua_State* L)
            {
                luabridge::getGlobalNamespace(L)

                    .beginNamespace("rpg")

                        .beginClass<CharacterWrapper>("Character")
                            .addProperty("id",                      &CharacterWrapper::get_id)
                            .addProperty("name",                    &CharacterWrapper::get_name)
                            .addProperty("charsetFilename",         &CharacterWrapper::get_charsetFilename)
                            .addProperty("charsetId",               &CharacterWrapper::get_charsetId)
                            .addProperty("screenX",                 &CharacterWrapper::get_screenX)
                            .addProperty("screenY",                 &CharacterWrapper::get_screenY)
                            .addProperty("enabled",                 &CharacterWrapper::get_enabled,              &CharacterWrapper::set_enabled)
                            .addProperty("x",                       &CharacterWrapper::get_x,                    &CharacterWrapper::set_x)
                            .addProperty("y",                       &CharacterWrapper::get_y,                    &CharacterWrapper::set_y)
                            .addProperty("offsetX",                 &CharacterWrapper::get_offsetX,              &CharacterWrapper::set_offsetX)
                            .addProperty("offsetY",                 &CharacterWrapper::get_offsetY,              &CharacterWrapper::set_offsetY)
                            .addProperty("direction",               &CharacterWrapper::get_direction,            &CharacterWrapper::set_direction)
                            .addProperty("facing",                  &CharacterWrapper::get_facing,               &CharacterWrapper::set_facing)
                            .addProperty("step",                    &CharacterWrapper::get_step,                 &CharacterWrapper::set_step)
                            .addProperty("transparency",            &CharacterWrapper::get_transparency,         &CharacterWrapper::set_transparency)
                            .addProperty("frequency",               &CharacterWrapper::get_frequency,            &CharacterWrapper::set_frequency)
                            .addProperty("layer",                   &CharacterWrapper::get_layer,                &CharacterWrapper::set_layer)
                            .addProperty("forbidEventOverlap",      &CharacterWrapper::get_forbidEventOverlap,   &CharacterWrapper::set_forbidEventOverlap)
                            .addProperty("animationType",           &CharacterWrapper::get_animationType,        &CharacterWrapper::set_animationType)
                            .addProperty("fixedDirection",          &CharacterWrapper::get_fixedDirection,       &CharacterWrapper::set_fixedDirection)
                            .addProperty("speed",                   &CharacterWrapper::get_speed,                &CharacterWrapper::set_speed)
                            .addProperty("moving",                  &CharacterWrapper::get_moving,               &CharacterWrapper::set_moving)
                            .addProperty("phasing",                 &CharacterWrapper::get_phasing,              &CharacterWrapper::set_phasing)
                            .addCFunction("getScreenPosition",      &CharacterWrapper::getScreenPosition)
                            .addCFunction("getPosition",            &CharacterWrapper::getPosition)
                            .addCFunction("setPosition",            &CharacterWrapper::setPosition)
                            .addCFunction("getPositionOffset",      &CharacterWrapper::getPositionOffset)
                            .addCFunction("setPositionOffset",      &CharacterWrapper::setPositionOffset)
                            .addCFunction("setFixedStep",           &CharacterWrapper::setFixedStep)
                            .addCFunction("setNormalStep",          &CharacterWrapper::setNormalStep)
                            .addCFunction("isMovePossible",         &CharacterWrapper::isMovePossible)
                            .addCFunction("stop",                   &CharacterWrapper::stop)
                            .addCFunction("doStep",                 &CharacterWrapper::doStep)
                        .endClass()

                        .deriveClass<EventWrapper, CharacterWrapper>("Event")
                        .endClass()

                        .deriveClass<HeroWrapper, CharacterWrapper>("Hero")
                            .addProperty("vehicle",                 &HeroWrapper::get_vehicle)
                            .addProperty("controlMode",             &HeroWrapper::get_controlMode,              &HeroWrapper::set_controlMode)
                        .endClass()

                        .beginClass<BattlerWrapper>("Battler")
                            .addProperty("databaseId",              &BattlerWrapper::get_databaseId)
                            .addProperty("name",                    &BattlerWrapper::get_name)
                            .addProperty("isMonster",               &BattlerWrapper::get_isMonster)
                            .addProperty("notHidden",               &BattlerWrapper::get_notHidden)
                            .addProperty("mirrored",                &BattlerWrapper::get_mirrored)
                            .addProperty("hp",                      &BattlerWrapper::get_hp,                    &BattlerWrapper::set_hp)
                            .addProperty("mp",                      &BattlerWrapper::get_mp,                    &BattlerWrapper::set_mp)
                            .addProperty("maxHp",                   &BattlerWrapper::get_maxHp)
                            .addProperty("maxMp",                   &BattlerWrapper::get_maxMp)
                            .addProperty("attack",                  &BattlerWrapper::get_attack)
                            .addProperty("defense",                 &BattlerWrapper::get_defense)
                            .addProperty("intelligence",            &BattlerWrapper::get_intelligence)
                            .addProperty("agility",                 &BattlerWrapper::get_agility)
                            .addProperty("atbValue",                &BattlerWrapper::get_atbValue)
                            .addProperty("turnsTaken",              &BattlerWrapper::get_turnsTaken)
                            .addProperty("screenX",                 &BattlerWrapper::get_screenX)
                            .addProperty("screenY",                 &BattlerWrapper::get_screenY)
                            .addCFunction("getScreenPosition",      &BattlerWrapper::getScreenPosition)
                        .endClass()

                        .deriveClass<MonsterWrapper, BattlerWrapper>("Monster")
                            .addProperty("monsterPartyId", &MonsterWrapper::get_monsterPartyId)
                        .endClass()

                        .deriveClass<ActorWrapper, BattlerWrapper>("Actor")
                            .addProperty("degree",                  &ActorWrapper::get_degree)
                            .addProperty("charsetFlename",          &ActorWrapper::get_charsetFilename)
                            .addProperty("charsetId",               &ActorWrapper::get_charsetId)
                            .addProperty("facesetFilename",         &ActorWrapper::get_facesetFilename)
                            .addProperty("facesetId",               &ActorWrapper::get_facesetId)
                            .addProperty("level",                   &ActorWrapper::get_level,                   &ActorWrapper::set_level)
                            .addProperty("exp",                     &ActorWrapper::get_exp,                     &ActorWrapper::set_exp)
                            .addProperty("weaponId",                &ActorWrapper::get_weaponId,                &ActorWrapper::set_weaponId)
                            .addProperty("shieldId",                &ActorWrapper::get_shieldId,                &ActorWrapper::set_shieldId)
                            .addProperty("armorId",                 &ActorWrapper::get_armorId,                 &ActorWrapper::set_armorId)
                            .addProperty("helmetId",                &ActorWrapper::get_helmetId,                &ActorWrapper::set_helmetId)
                            .addProperty("accessoryId",             &ActorWrapper::get_accessoryId,             &ActorWrapper::set_accessoryId)
                            .addProperty("twoWeapons",              &ActorWrapper::get_twoWeapons,              &ActorWrapper::set_twoWeapons)
                            .addProperty("lockEquipment",           &ActorWrapper::get_lockEquipment,           &ActorWrapper::set_lockEquipment)
                            .addProperty("autoBattle",              &ActorWrapper::get_autoBattle,              &ActorWrapper::set_autoBattle)
                            .addProperty("mightyGuard",             &ActorWrapper::get_mightyGuard,             &ActorWrapper::set_mightyGuard)
                        .endClass()

                    .endNamespace()

                    .beginNamespace("game")

                        .addProperty("fps",                 &game_get_fps,                  &game_set_fps)
                        .addProperty("millisPerFrame",      &game_get_millisPerFrame)
                        .addProperty("frameCounter",        &game_get_frameCounter)
                        .addProperty("scene",               &game_get_scene)
                        .addProperty("isTestPlay",          &game_get_isTestPlay)
                        .addProperty("isBattleTest",        &game_get_isBattleTest)
                        .addProperty("saveCount",           &game_get_saveCount)
                        .addProperty("variables",           &game_get_variables)
                        .addProperty("switches",            &game_get_switches)
                        .addProperty("actors",              &game_get_actors)
                        .addProperty("party",               &game_get_party)

                        .beginNamespace("menu")
                            .addProperty("scene",           &game_menu_get_scene)
                            .addProperty("allowed",         &game_menu_get_allowed,         &game_menu_set_allowed)
                            .addProperty("saveAllowed",     &game_menu_get_saveAllowed,     &game_menu_set_saveAllowed)
                            .addProperty("teleportAllowed", &game_menu_get_teleportAllowed, &game_menu_set_teleportAllowed)
                            .addProperty("escapeAllowed",   &game_menu_get_escapeAllowed,   &game_menu_set_escapeAllowed)
                        .endNamespace()

                        .beginNamespace("map")
                            .addProperty("hero",                    &game_map_get_hero)
                            .addProperty("events",                  &game_map_get_events)
                            .addProperty("id",                      &game_map_get_id)
                            .addProperty("width",                   &game_map_get_width)
                            .addProperty("height",                  &game_map_get_height)
                            .addProperty("cameraX",                 &game_map_get_cameraX)
                            .addProperty("cameraY",                 &game_map_get_cameraY)
                            .addCFunction("existsEvent",            &game_map_existsEvent)
                            .addCFunction("findEvent",              &game_map_findEvent)
                            .addCFunction("updateEvents",           &game_map_updateEvents)
                            .addCFunction("getCameraPosition",      &game_map_getCameraPosition)
                            .addCFunction("setCameraPosition",      &game_map_setCameraPosition)
                            .addCFunction("moveCamera",             &game_map_moveCamera)
                            .addCFunction("getLowerLayerTileId",    &game_map_getLowerLayerTileId)
                            .addCFunction("getUpperLayerTileId",    &game_map_getUpperLayerTileId)
                            .addCFunction("getTerrainId",           &game_map_getTerrainId)
                        .endNamespace()

                        .beginNamespace("battle")
                            .addProperty("enemies",     &game_battle_get_enemies)
                        .endNamespace()

                        .beginNamespace("input")
                            .addCFunction("pressed",    &game_input_pressed)
                            .addCFunction("assign",     &game_input_assign)
                        .endNamespace()

                        .beginNamespace("music")
                            .addCFunction("play",       &game_music_play)
                            .addCFunction("fadeOut",    &game_music_fadeOut)
                            .addCFunction("stop",       &game_music_stop)
                        .endNamespace()

                        .beginNamespace("sound")
                            .addCFunction("play",       &game_sound_play)
                            .addCFunction("stop",       &game_sound_stop)
                        .endNamespace()

                        .beginNamespace("screen")
                            .addProperty("width",                   &game_screen_get_width)
                            .addProperty("height",                  &game_screen_get_height)
                            .addProperty("brightness",              &game_screen_get_brightness, &game_screen_set_brightness)
                            .addCFunction("getDimensions",          &game_screen_getDimensions)
                            .addCFunction("getClipRect",            &game_screen_getClipRect)
                            .addCFunction("setClipRect",            &game_screen_setClipRect)
                            .addCFunction("copyRect",               &game_screen_copyRect)
                            .addCFunction("clear",                  &game_screen_clear)
                            .addCFunction("grey",                   &game_screen_grey)
                            .addCFunction("getPixel",               &game_screen_getPixel)
                            .addCFunction("setPixel",               &game_screen_setPixel)
                            .addCFunction("drawPoint",              &game_screen_drawPoint)
                            .addCFunction("drawLine",               &game_screen_drawLine)
                            .addCFunction("drawRectangle",          &game_screen_drawRectangle)
                            .addCFunction("drawCircle",             &game_screen_drawCircle)
                            .addCFunction("drawTriangle",           &game_screen_drawTriangle)
                            .addCFunction("draw",                   &game_screen_draw)
                            .addCFunction("drawq",                  &game_screen_drawq)
                            .addCFunction("drawText",               &game_screen_drawText)
                            .addCFunction("drawWindow",             &game_screen_drawWindow)
                        .endNamespace()

                    .endNamespace();

                return true;
            }

        } // game_module
    } // namespace script
} // namespace rpgss
