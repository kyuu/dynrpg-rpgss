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

#include <emmintrin.h>

#define NOT_MAIN_MODULE
#include <DynRPG/DynRPG.h>

#include "../../Context.hpp"
#include "../../common/types.hpp"
#include "../../common/cpuinfo.hpp"
#include "../../graphics/primitives.hpp"
#include "../graphics_module/graphics_module.hpp"
#include "../core_module/core_module.hpp"
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

} // namespace RPG

namespace rpgss {
    namespace script {
        namespace game_module {

            /**********************************************************
             *                          MAP
             **********************************************************/

            //---------------------------------------------------------
            luabridge::LuaRef map_get_hero()
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
            int map_events_proxy_table_indexMetaMethod(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                if (n < 1 || n > RPG::map->events.count()) {
                   return luaL_error(L, "index out of bounds");
                }
                luabridge::push(L, EventWrapper(RPG::map->events.ptr->list[n-1]));
                return 1;
            }

            //---------------------------------------------------------
            int map_events_proxy_table_newindexMetaMethod(lua_State* L)
            {
               return luaL_error(L, "operation not allowed");
            }

            //---------------------------------------------------------
            int map_events_proxy_table_lenMetaMethod(lua_State* L)
            {
                lua_pushnumber(L, RPG::map->events.count());
                return 1;
            }

            //---------------------------------------------------------
            luabridge::LuaRef map_get_events()
            {
                lua_State* L = Context::Current().interpreter();

                lua_pushstring(L, "__game_map_events_proxy_table");
                lua_rawget(L, LUA_REGISTRYINDEX); // get proxy table from registry table

                if (!lua_istable(L, -1)) {
                    lua_newtable(L); // proxy table

                    lua_newtable(L); // metatable

                    lua_pushstring(L, "__index");
                    lua_pushcfunction(L, map_events_proxy_table_indexMetaMethod);
                    lua_rawset(L, -3); // add __index() to metatable

                    lua_pushstring(L, "__newindex");
                    lua_pushcfunction(L, map_events_proxy_table_newindexMetaMethod);
                    lua_rawset(L, -3); // add __newindex() to metatable

                    lua_pushstring(L, "__len");
                    lua_pushcfunction(L, map_events_proxy_table_lenMetaMethod);
                    lua_rawset(L, -3); // add __len() to metatable

                    lua_setmetatable(L, -2); // set metatable for proxy table

                    lua_pushstring(L, "__game_map_events_proxy_table");
                    lua_pushvalue(L, -2); // push proxy table
                    lua_rawset(L, LUA_REGISTRYINDEX); // store proxy table in registry table

                    assert(lua_istable(L, -1));
                }

                return luabridge::LuaRef::fromStack(L, -1);
            }

            //---------------------------------------------------------
            int map_get_id()
            {
                return RPG::hero->mapId;
            }

            //---------------------------------------------------------
            int map_get_width()
            {
                return RPG::map->getWidth();
            }

            //---------------------------------------------------------
            int map_get_height()
            {
                return RPG::map->getHeight();
            }

            //---------------------------------------------------------
            int map_get_cameraX()
            {
                return RPG::map->getCameraX();
            }

            //---------------------------------------------------------
            int map_get_cameraY()
            {
                return RPG::map->getCameraY();
            }

            //---------------------------------------------------------
            int map_findEventById(lua_State* L)
            {
                int id = luaL_checkint(L, 1);
                RPG::Event* event = RPG::map->events.get(id);
                if (event) {
                    luabridge::push(L, EventWrapper(event));
                } else {
                    lua_pushnil(L);
                }
                return 1;
            }

            //---------------------------------------------------------
            int map_findEventByName(lua_State* L)
            {
                const char* name = luaL_checkstring(L, 1);
                for (int i = 0; i < RPG::map->events.count(); ++i) {
                    RPG::Event* event = RPG::map->events.ptr->list[i];
                    if (std::strcmp(event->getName().c_str(), name) == 0) {
                        luabridge::push(L, EventWrapper(event));
                        return 1;
                    }
                }
                lua_pushnil(L);
                return 1;
            }

            //---------------------------------------------------------
            int map_updateEvents(lua_State* L)
            {
                RPG::map->updateEvents();
                return 0;
            }

            //---------------------------------------------------------
            int map_getCameraPosition(lua_State* L)
            {
                lua_pushnumber(L, RPG::map->getCameraX());
                lua_pushnumber(L, RPG::map->getCameraY());
                return 2;
            }

            //---------------------------------------------------------
            int map_setCameraPosition(lua_State* L)
            {
                int x = luaL_checkint(L, 1);
                int y = luaL_checkint(L, 2);
                RPG::map->setCamera(x, y);
                return 0;
            }

            //---------------------------------------------------------
            int map_moveCamera(lua_State* L)
            {
                int ox = luaL_checkint(L, 1);
                int oy = luaL_checkint(L, 2);
                int speed = luaL_checkint(L, 3);
                RPG::map->moveCamera(ox, oy, speed);
                return 0;
            }

            //---------------------------------------------------------
            int map_getLowerLayerTileId(lua_State* L)
            {
                int x = luaL_checkint(L, 1);
                int y = luaL_checkint(L, 2);

                luaL_argcheck(L, x >= 0 && x < RPG::map->getWidth(), 1, "invalid x");
                luaL_argcheck(L, y >= 0 && y < RPG::map->getHeight(), 2, "invalid y");

                lua_pushnumber(L, RPG::getLowerLayerTileId(RPG::map, x, y));
                return 1;
            }

            //---------------------------------------------------------
            int map_getUpperLayerTileId(lua_State* L)
            {
                int x = luaL_checkint(L, 1);
                int y = luaL_checkint(L, 2);

                luaL_argcheck(L, x >= 0 && x < RPG::map->getWidth(), 1, "invalid x");
                luaL_argcheck(L, y >= 0 && y < RPG::map->getHeight(), 2, "invalid y");

                lua_pushnumber(L, RPG::getUpperLayerTileId(RPG::map, x, y));
                return 1;
            }

            //---------------------------------------------------------
            int map_getTerrainId(lua_State* L)
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
            int battle_monster_party_proxy_table_indexMetaMethod(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                if (n < 1 || n > RPG::monsters.count()) {
                   return luaL_error(L, "index out of bounds");
                }
                luabridge::push(L, MonsterWrapper(RPG::monsters[n-1]));
                return 1;
            }

            //---------------------------------------------------------
            int battle_monster_party_proxy_table_newindexMetaMethod(lua_State* L)
            {
               return luaL_error(L, "operation not allowed");
            }

            //---------------------------------------------------------
            int battle_monster_party_proxy_table_lenMetaMethod(lua_State* L)
            {
                lua_pushnumber(L, RPG::monsters.count());
                return 1;
            }

            //---------------------------------------------------------
            luabridge::LuaRef battle_get_monsterParty()
            {
                lua_State* L = Context::Current().interpreter();

                lua_pushstring(L, "__game_battle_monster_party_proxy_table");
                lua_rawget(L, LUA_REGISTRYINDEX); // get proxy table from registry table

                if (!lua_istable(L, -1)) {
                    lua_newtable(L); // proxy table

                    lua_newtable(L); // metatable

                    lua_pushstring(L, "__index");
                    lua_pushcfunction(L, battle_monster_party_proxy_table_indexMetaMethod);
                    lua_rawset(L, -3); // add __index() to metatable

                    lua_pushstring(L, "__newindex");
                    lua_pushcfunction(L, battle_monster_party_proxy_table_newindexMetaMethod);
                    lua_rawset(L, -3); // add __newindex() to metatable

                    lua_pushstring(L, "__len");
                    lua_pushcfunction(L, battle_monster_party_proxy_table_lenMetaMethod);
                    lua_rawset(L, -3); // add __len() to metatable

                    lua_setmetatable(L, -2); // set metatable for proxy table

                    lua_pushstring(L, "__game_battle_monster_party_proxy_table");
                    lua_pushvalue(L, -2); // push proxy table
                    lua_rawset(L, LUA_REGISTRYINDEX); // store proxy table in registry table

                    assert(lua_istable(L, -1));
                }

                return luabridge::LuaRef::fromStack(L, -1);
            }

            /***********************************************************
             *                          INPUT
             **********************************************************/

            //---------------------------------------------------------
            int input_pressed(lua_State* L)
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
            int input_assign(lua_State* L)
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
            int music_playSystemBGM(lua_State* L)
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
            int music_play(lua_State* L)
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
            int music_fadeOut(lua_State* L)
            {
                int fade_out_time = luaL_checkint(L, 1);
                RPG::Music::fadeOut(fade_out_time);
                return 0;
            }

            //---------------------------------------------------------
            int music_stop(lua_State* L)
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
            int sound_playSystemSE(lua_State* L)
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
            int sound_play(lua_State* L)
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
            int sound_stop(lua_State* L)
            {
                RPG::Sound::stop();
                return 0;
            }

            /***********************************************************
             *                        SCREEN
             **********************************************************/

            //---------------------------------------------------------
            struct Screen
            {
                //---------------------------------------------------------
                static u16* Pixels()
                {
                    return RPG::screen->canvas->getScanline(0);
                }

                //---------------------------------------------------------
                static int Width()
                {
                    return RPG::screen->canvas->width();
                }

                //---------------------------------------------------------
                static int Height()
                {
                    return RPG::screen->canvas->height();
                }

                //---------------------------------------------------------
                static int Pitch()
                {
                    return RPG::screen->canvas->lineSize / 2;
                }

                static core::Recti ClipRect;
            };

            //---------------------------------------------------------
            core::Recti Screen::ClipRect = core::Recti(0, 0, 320, 240);

            //---------------------------------------------------------
            int screen_get_width()
            {
                return RPG::screen->canvas->width();
            }

            //---------------------------------------------------------
            int screen_get_height()
            {
                return RPG::screen->canvas->height();
            }

            //---------------------------------------------------------
            int screen_get_brightness()
            {
                return RPG::screen->canvas->brightness;
            }

            //---------------------------------------------------------
            void screen_set_brightness(int brightness)
            {
                RPG::screen->canvas->brightness = brightness;
            }

            //---------------------------------------------------------
            int screen_getDimensions(lua_State* L)
            {
                lua_pushnumber(L, RPG::screen->canvas->width());
                lua_pushnumber(L, RPG::screen->canvas->height());
                return 2;
            }

            //---------------------------------------------------------
            int screen_getClipRect(lua_State* L)
            {
                lua_pushnumber(L, Screen::ClipRect.getX());
                lua_pushnumber(L, Screen::ClipRect.getY());
                lua_pushnumber(L, Screen::ClipRect.getWidth());
                lua_pushnumber(L, Screen::ClipRect.getHeight());
                return 4;
            }

            //---------------------------------------------------------
            int screen_setClipRect(lua_State* L)
            {
                int x = luaL_checkint(L, 1);
                int y = luaL_checkint(L, 2);
                int w = luaL_checkint(L, 3);
                int h = luaL_checkint(L, 4);

                core::Recti clip_rect = core::Recti(x, y, w, h);
                core::Recti screen_bounds = core::Recti(
                    RPG::screen->canvas->width(),
                    RPG::screen->canvas->height()
                );

                if (!clip_rect.isValid() || !clip_rect.isInside(screen_bounds)) {
                    return luaL_error(L, "invalid rect");
                }

                Screen::ClipRect = clip_rect;
                return 0;
            }

            //---------------------------------------------------------
            int screen_copyRect(lua_State* L)
            {
                int x = luaL_checkint(L, 1);
                int y = luaL_checkint(L, 2);
                int w = luaL_checkint(L, 3);
                int h = luaL_checkint(L, 4);
                graphics::Image* destination = graphics_module::ImageWrapper::GetOpt(L, 5);

                core::Recti rect(x, y, w, h);
                core::Recti screen_bounds(RPG::screen->canvas->width(), RPG::screen->canvas->height());

                if (!rect.isValid() || !rect.isInside(screen_bounds)) {
                    return luaL_error(L, "invalid rect");
                }

                graphics::Image::Ptr image;

                if (destination) {
                    image = destination;
                    image->resize(w, h);
                } else {
                    image = graphics::Image::New(w, h);
                }

                graphics::RGBA* dst = image->getPixels();

                int  src_pitch = Screen::Pitch();
                u16* src       = Screen::Pixels() + src_pitch * y + x;

                for (int iy = 0; iy < h; iy++) {
                    for (int ix = 0; ix < w; ix++) {
                        *dst = graphics::RGB565ToRGBA(*src);
                        src++;
                        dst++;
                    }
                    src += src_pitch - w;
                }

                graphics_module::ImageWrapper::Push(L, image);
                return 1;
            }

            //---------------------------------------------------------
            void screen_clear_generic(u16 color)
            {
                u16* dp = Screen::Pixels();
                int  di = Screen::Pitch() - Screen::Width();

                for (int y = Screen::Height(); y > 0; y--) {
                    for (int x = Screen::Width(); x > 0; x--) {
                        *dp = color;
                        dp++;
                    }
                    dp += di;
                }
            }

            //---------------------------------------------------------
            void screen_clear_sse2(u16 color)
            {
                u16* dp = Screen::Pixels();
                int  di = Screen::Pitch() - Screen::Width();

                int burst1 = RPG::screen->canvas->width() / 8;
                int burst2 = RPG::screen->canvas->width() % 8;

                __m128i mcolor = _mm_set_epi16(
                    color, color, color, color,
                    color, color, color, color
                );

                for (int y = Screen::Height(); y > 0; y--) {
                    for (int x = burst1; x > 0; x--) {
                        _mm_storeu_si128((__m128i*)dp, mcolor);
                        dp += 8;
                    }
                    for (int x = burst2; x > 0; x--) {
                        *dp = color;
                        dp++;
                    }
                    dp += di;
                }
            }

            //---------------------------------------------------------
            int screen_clear(lua_State* L)
            {
                u32 c = (u32)luaL_optint(L, 1, 0x000000FF);

                if (CpuSupportsSse2())
                {
                    screen_clear_sse2(graphics::RGBA8888ToRGB565(c));
                }
                else
                {
                    screen_clear_generic(graphics::RGBA8888ToRGB565(c));
                }

                return 0;
            }

            //---------------------------------------------------------
            int screen_grey(lua_State* L)
            {
                int  dst_pitch  = Screen::Pitch();
                u16* dst_pixels = Screen::Pixels();

                int w = RPG::screen->canvas->width();
                int h = RPG::screen->canvas->height();

                for (int y = 0; y < h; y++) {
                    u16* dst = dst_pixels + dst_pitch * y;
                    for (int x = 0; x < w; x++) {
                        unsigned int c = *dst;
                        unsigned int g = ((c >> 11) + ((c >> 6) & 0x001F) + (c & 0x001F)) / 3;
                        *dst = (g << 11) | ((g << 6) | 0x01)  | g;
                        dst++;
                    }
                }

                return 0;
            }

            //---------------------------------------------------------
            int screen_getPixel(lua_State* L)
            {
                int x = luaL_checkint(L, 1);
                int y = luaL_checkint(L, 2);

                luaL_argcheck(L, x >= 0 && x < RPG::screen->canvas->width(),  1, "invalid x");
                luaL_argcheck(L, y >= 0 && y < RPG::screen->canvas->height(), 2, "invalid y");

                u16 c = *(Screen::Pixels() + Screen::Pitch() * y + x);
                lua_pushinteger(L, (i32)graphics::RGB565ToRGBA8888(c));

                return 1;
            }

            //---------------------------------------------------------
            int screen_setPixel(lua_State* L)
            {
                int x = luaL_checkint(L, 1);
                int y = luaL_checkint(L, 2);
                u32 c = (u32)luaL_checkint(L, 3);

                luaL_argcheck(L, x >= 0 && x < RPG::screen->canvas->width(),  1, "invalid x");
                luaL_argcheck(L, y >= 0 && y < RPG::screen->canvas->height(), 2, "invalid y");

                *(Screen::Pixels() + Screen::Pitch() * y + x) = graphics::RGBA8888ToRGB565(c);

                return 0;
            }

            //---------------------------------------------------------
            struct rgb565_set
            {
                void operator()(u16& dst, const graphics::RGBA& src)
                {
                    asm (
                        "leal           %1,   %%edx   \n\t"
                        "movzbl    (%%edx),   %%eax   \n\t"
                        "movzbl   1(%%edx),   %%ecx   \n\t"
                        "movzbl   2(%%edx),   %%edx   \n\t"
                        "andl         $248,   %%eax   \n\t"
                        "sall           $8,   %%eax   \n\t"
                        "sarl           $3,   %%edx   \n\t"
                        "andl         $252,   %%ecx   \n\t"
                        "sall           $3,   %%ecx   \n\t"
                        "orl         %%ecx,   %%eax   \n\t"
                        "orl         %%edx,   %%eax   \n\t"
                        "leal           %0,   %%edx   \n\t"
                        "movw         %%ax, (%%edx)"
                        :
                        : "m"(dst), "m"(src)
                        : "eax", "ecx", "edx"
                    );
                    // C++ version:
                    //dst = ((src.red & 0xF8) << 8) | ((src.green & 0xFC) << 3) | (src.blue >> 3);
                }

                void operator()(u16& dst, u8 red, u8 green, u8 blue, u8 alpha)
                {
                    dst = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
                }
            };

            //---------------------------------------------------------
            struct rgb565_mix
            {
                void operator()(u16& dst, const graphics::RGBA& src)
                {
                    graphics::RGBA temp;
                    int sa = src.alpha + 1;
                    int da = 256 - src.alpha;
                    temp.red   = (   (dst >>  11)         * da + (src.red   >> 3) * sa ) >> 8;
                    temp.green = ( ( (dst >>   5) & 0x3F) * da + (src.green >> 2) * sa ) >> 8;
                    temp.blue  = ( (  dst         & 0x1F) * da + (src.blue  >> 3) * sa ) >> 8;
                    dst = (temp.red << 11) | (temp.green << 5) | temp.blue;
                }

                void operator()(u16& dst, u8 red, u8 green, u8 blue, u8 alpha)
                {
                    graphics::RGBA temp;
                    int sa = alpha + 1;
                    int da = 256 - alpha;
                    temp.red   = (   (dst >> 11)         * da + (red   >> 3) * sa ) >> 8;
                    temp.green = ( ( (dst >>  5) & 0x3F) * da + (green >> 2) * sa ) >> 8;
                    temp.blue  = ( (  dst        & 0x1F) * da + (blue  >> 3) * sa ) >> 8;
                    dst = (temp.red << 11) | (temp.green << 5) | temp.blue;
                }
            };

            //---------------------------------------------------------
            struct rgb565_add
            {
                void operator()(u16& dst, const graphics::RGBA& src)
                {
                    u8 r = std::min(  (dst >> 11)         + (src.red   >> 3), 31);
                    u8 g = std::min(( (dst >>  5) & 0x3F) + (src.green >> 2), 63);
                    u8 b = std::min((  dst        & 0x1F) + (src.blue  >> 3), 31);
                    dst = (r << 11) | (g << 5) | b;
                }

                void operator()(u16& dst, u8 red, u8 green, u8 blue, u8 alpha)
                {
                    u8 r = std::min(  (dst >> 11)         + (red   >> 3), 31);
                    u8 g = std::min(( (dst >>  5) & 0x3F) + (green >> 2), 63);
                    u8 b = std::min((  dst        & 0x1F) + (blue  >> 3), 31);
                    dst = (r << 11) | (g << 5) | b;
                }
            };

            //---------------------------------------------------------
            struct rgb565_sub
            {
                void operator()(u16& dst, const graphics::RGBA& src)
                {
                    u8 r = std::max(  (dst >> 11)         - (src.red   >> 3), 0);
                    u8 g = std::max(( (dst >>  5) & 0x3F) - (src.green >> 2), 0);
                    u8 b = std::max((  dst        & 0x1F) - (src.blue  >> 3), 0);
                    dst = (r << 11) | (g << 5) | b;
                }

                void operator()(u16& dst, u8 red, u8 green, u8 blue, u8 alpha)
                {
                    u8 r = std::max(  (dst >> 11)         - (red   >> 3), 0);
                    u8 g = std::max(( (dst >>  5) & 0x3F) - (green >> 2), 0);
                    u8 b = std::max((  dst        & 0x1F) - (blue  >> 3), 0);
                    dst = (r << 11) | (g << 5) | b;
                }
            };

            //---------------------------------------------------------
            struct rgb565_mul
            {
                void operator()(u16& dst, const graphics::RGBA& src)
                {
                    u8 r =   (dst >> 11)         * ((src.red   >> 3) + 1) >> 5;
                    u8 g = ( (dst >>  5) & 0x3F) * ((src.green >> 2) + 1) >> 6;
                    u8 b = (  dst        & 0x1F) * ((src.blue  >> 3) + 1) >> 5;
                    dst = (r << 11) | (g << 5) | b;
                }

                void operator()(u16& dst, u8 red, u8 green, u8 blue, u8 alpha)
                {
                    u8 r =   (dst >> 11)         * ((red   >> 3) + 1) >> 5;
                    u8 g = ( (dst >>  5) & 0x3F) * ((green >> 2) + 1) >> 6;
                    u8 b = (  dst        & 0x1F) * ((blue  >> 3) + 1) >> 5;
                    dst = (r << 11) | (g << 5) | b;
                }
            };

            //---------------------------------------------------------
            int screen_drawPoint(lua_State* L)
            {
                int x = luaL_checkint(L, 1);
                int y = luaL_checkint(L, 2);
                u32 c = (u32)luaL_checkint(L, 3);

                int blend_mode;
                const char* blend_mode_str = luaL_optstring(L, 4, "mix");
                if (!graphics_module::GetBlendModeConstant(blend_mode_str, blend_mode)) {
                    return luaL_argerror(L, 4, "invalid blend mode constant");
                }

                switch (blend_mode) {
                case graphics::BlendMode::Set:      graphics::primitives::Point(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, core::Vec2i(x, y), graphics::RGBA8888ToRGBA(c), rgb565_set()); break;
                case graphics::BlendMode::Mix:      graphics::primitives::Point(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, core::Vec2i(x, y), graphics::RGBA8888ToRGBA(c), rgb565_mix()); break;
                case graphics::BlendMode::Add:      graphics::primitives::Point(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, core::Vec2i(x, y), graphics::RGBA8888ToRGBA(c), rgb565_add()); break;
                case graphics::BlendMode::Subtract: graphics::primitives::Point(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, core::Vec2i(x, y), graphics::RGBA8888ToRGBA(c), rgb565_sub()); break;
                case graphics::BlendMode::Multiply: graphics::primitives::Point(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, core::Vec2i(x, y), graphics::RGBA8888ToRGBA(c), rgb565_mul()); break;
                }

                return 0;
            }

            //---------------------------------------------------------
            int screen_drawLine(lua_State* L)
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

                switch (blend_mode) {
                case graphics::BlendMode::Set:      graphics::primitives::Line(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, core::Vec2i(x1, y1), core::Vec2i(x2, y2), graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), rgb565_set()); break;
                case graphics::BlendMode::Mix:      graphics::primitives::Line(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, core::Vec2i(x1, y1), core::Vec2i(x2, y2), graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), rgb565_mix()); break;
                case graphics::BlendMode::Add:      graphics::primitives::Line(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, core::Vec2i(x1, y1), core::Vec2i(x2, y2), graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), rgb565_add()); break;
                case graphics::BlendMode::Subtract: graphics::primitives::Line(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, core::Vec2i(x1, y1), core::Vec2i(x2, y2), graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), rgb565_sub()); break;
                case graphics::BlendMode::Multiply: graphics::primitives::Line(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, core::Vec2i(x1, y1), core::Vec2i(x2, y2), graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), rgb565_mul()); break;
                }

                return 0;
            }

            //---------------------------------------------------------
            int screen_drawRectangle(lua_State* L)
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

                switch (blend_mode) {
                case graphics::BlendMode::Set:      graphics::primitives::Rectangle(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, fill, core::Recti(x, y, w, h), graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), graphics::RGBA8888ToRGBA(c3), graphics::RGBA8888ToRGBA(c4), rgb565_set()); break;
                case graphics::BlendMode::Mix:      graphics::primitives::Rectangle(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, fill, core::Recti(x, y, w, h), graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), graphics::RGBA8888ToRGBA(c3), graphics::RGBA8888ToRGBA(c4), rgb565_mix()); break;
                case graphics::BlendMode::Add:      graphics::primitives::Rectangle(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, fill, core::Recti(x, y, w, h), graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), graphics::RGBA8888ToRGBA(c3), graphics::RGBA8888ToRGBA(c4), rgb565_add()); break;
                case graphics::BlendMode::Subtract: graphics::primitives::Rectangle(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, fill, core::Recti(x, y, w, h), graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), graphics::RGBA8888ToRGBA(c3), graphics::RGBA8888ToRGBA(c4), rgb565_sub()); break;
                case graphics::BlendMode::Multiply: graphics::primitives::Rectangle(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, fill, core::Recti(x, y, w, h), graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), graphics::RGBA8888ToRGBA(c3), graphics::RGBA8888ToRGBA(c4), rgb565_mul()); break;
                }

                return 0;
            }

            //---------------------------------------------------------
            int screen_drawCircle(lua_State* L)
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

                switch (blend_mode) {
                case graphics::BlendMode::Set:      graphics::primitives::Circle(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, fill, core::Vec2i(x, y), r, graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), rgb565_set()); break;
                case graphics::BlendMode::Mix:      graphics::primitives::Circle(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, fill, core::Vec2i(x, y), r, graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), rgb565_mix()); break;
                case graphics::BlendMode::Add:      graphics::primitives::Circle(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, fill, core::Vec2i(x, y), r, graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), rgb565_add()); break;
                case graphics::BlendMode::Subtract: graphics::primitives::Circle(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, fill, core::Vec2i(x, y), r, graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), rgb565_sub()); break;
                case graphics::BlendMode::Multiply: graphics::primitives::Circle(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, fill, core::Vec2i(x, y), r, graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), rgb565_mul()); break;
                }

                return 0;
            }

            //---------------------------------------------------------
            int screen_drawTriangle(lua_State* L)
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
            struct rgb565_set_col
            {
                graphics::RGBA c;

                explicit rgb565_set_col(graphics::RGBA color)
                    : c(color)
                {
                    c.red   = (c.red   >> 3) + 1;
                    c.green = (c.green >> 2) + 1;
                    c.blue  = (c.blue  >> 3) + 1;
                }

                void operator()(u16& dst, const graphics::RGBA& src)
                {
                    u8 r = ((src.red   >> 3) * c.red  ) >> 5;
                    u8 g = ((src.green >> 2) * c.green) >> 6;
                    u8 b = ((src.blue  >> 3) * c.blue ) >> 5;
                    dst = (r << 11) | (g << 5) | b;
                }
            };

            //---------------------------------------------------------
            struct rgb565_mix_col
            {
                graphics::RGBA c;

                explicit rgb565_mix_col(graphics::RGBA color)
                    : c(color)
                {
                    c.red   = (c.red   >> 3) + 1;
                    c.green = (c.green >> 2) + 1;
                    c.blue  = (c.blue  >> 3) + 1;
                }

                void operator()(u16& dst, const graphics::RGBA& src)
                {
                    int sa = (src.alpha * (c.alpha + 1) >> 8) + 1;
                    int da = 256 - (sa - 1);
                    u8 r = (   (dst >>  11)         * da + (((src.red   >> 3) * c.red  ) >> 5) * sa ) >> 8;
                    u8 g = ( ( (dst >>   5) & 0x3F) * da + (((src.green >> 2) * c.green) >> 6) * sa ) >> 8;
                    u8 b = ( (  dst         & 0x1F) * da + (((src.blue  >> 3) * c.blue ) >> 5) * sa ) >> 8;
                    dst = (r << 11) | (g << 5) | b;
                }
            };

            //---------------------------------------------------------
            struct rgb565_add_col
            {
                graphics::RGBA c;

                explicit rgb565_add_col(graphics::RGBA color)
                    : c(color)
                {
                    c.red   = (c.red   >> 3) + 1;
                    c.green = (c.green >> 2) + 1;
                    c.blue  = (c.blue  >> 3) + 1;
                }

                void operator()(u16& dst, const graphics::RGBA& src)
                {
                    u8 r = std::min(   (dst >> 11)         + (((src.red   >> 3) * c.red  ) >> 5), 31 );
                    u8 g = std::min( ( (dst >>  5) & 0x3F) + (((src.green >> 2) * c.green) >> 6), 63 );
                    u8 b = std::min( (  dst        & 0x1F) + (((src.blue  >> 3) * c.blue ) >> 5), 31 );
                    dst = (r << 11) | (g << 5) | b;
                }
            };

            //---------------------------------------------------------
            struct rgb565_sub_col
            {
                graphics::RGBA c;

                explicit rgb565_sub_col(graphics::RGBA color)
                    : c(color)
                {
                    c.red   = (c.red   >> 3) + 1;
                    c.green = (c.green >> 2) + 1;
                    c.blue  = (c.blue  >> 3) + 1;
                }

                void operator()(u16& dst, const graphics::RGBA& src)
                {
                    u8 r = std::max(   (dst >> 11)         - (((src.red   >> 3) * c.red  ) >> 5), 0 );
                    u8 g = std::max( ( (dst >>  5) & 0x3F) - (((src.green >> 2) * c.green) >> 6), 0 );
                    u8 b = std::max( (  dst        & 0x1F) - (((src.blue  >> 3) * c.blue ) >> 5), 0 );
                    dst = (r << 11) | (g << 5) | b;
                }
            };

            //---------------------------------------------------------
            struct rgb565_mul_col
            {
                graphics::RGBA c;

                explicit rgb565_mul_col(graphics::RGBA color)
                    : c(color)
                {
                    c.red   = (c.red   >> 3) + 1;
                    c.green = (c.green >> 2) + 1;
                    c.blue  = (c.blue  >> 3) + 1;
                }

                void operator()(u16& dst, const graphics::RGBA& src)
                {
                    u8 r =   (dst >> 11)         * ((((src.red   >> 3) * c.red  ) >> 5) + 1) >> 5;
                    u8 g = ( (dst >>  5) & 0x3F) * ((((src.green >> 2) * c.green) >> 6) + 1) >> 6;
                    u8 b = (  dst        & 0x1F) * ((((src.blue  >> 3) * c.blue ) >> 5) + 1) >> 5;
                    dst = (r << 11) | (g << 5) | b;
                }
            };

            //---------------------------------------------------------
            int screen_draw(lua_State* L)
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

                if (angle == 0.0)
                {
                    if (color == 0xFFFFFFFF)
                    {
                        switch (blend_mode) {
                        case graphics::BlendMode::Set:      graphics::primitives::TexturedRectangle(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, core::Recti(x, y, sw, sh).scale(scale), that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_set()); break;
                        case graphics::BlendMode::Mix:      graphics::primitives::TexturedRectangle(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, core::Recti(x, y, sw, sh).scale(scale), that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_mix()); break;
                        case graphics::BlendMode::Add:      graphics::primitives::TexturedRectangle(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, core::Recti(x, y, sw, sh).scale(scale), that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_add()); break;
                        case graphics::BlendMode::Subtract: graphics::primitives::TexturedRectangle(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, core::Recti(x, y, sw, sh).scale(scale), that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_sub()); break;
                        case graphics::BlendMode::Multiply: graphics::primitives::TexturedRectangle(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, core::Recti(x, y, sw, sh).scale(scale), that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_mul()); break;
                        }
                    }
                    else
                    {
                        switch (blend_mode) {
                        case graphics::BlendMode::Set:      graphics::primitives::TexturedRectangle(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, core::Recti(x, y, sw, sh).scale(scale), that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_set_col(graphics::RGBA8888ToRGBA(color))); break;
                        case graphics::BlendMode::Mix:      graphics::primitives::TexturedRectangle(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, core::Recti(x, y, sw, sh).scale(scale), that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_mix_col(graphics::RGBA8888ToRGBA(color))); break;
                        case graphics::BlendMode::Add:      graphics::primitives::TexturedRectangle(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, core::Recti(x, y, sw, sh).scale(scale), that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_add_col(graphics::RGBA8888ToRGBA(color))); break;
                        case graphics::BlendMode::Subtract: graphics::primitives::TexturedRectangle(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, core::Recti(x, y, sw, sh).scale(scale), that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_sub_col(graphics::RGBA8888ToRGBA(color))); break;
                        case graphics::BlendMode::Multiply: graphics::primitives::TexturedRectangle(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, core::Recti(x, y, sw, sh).scale(scale), that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_mul_col(graphics::RGBA8888ToRGBA(color))); break;
                        }
                    }
                }
                else
                {
                    core::Recti rect = core::Recti(x, y, sw, sh).scale(scale);
                    core::Vec2i center_of_rotation = rect.getCenter();

                    core::Vec2i vertices[4] = {
                        rect.getUpperLeft().rotateBy(angle, center_of_rotation),
                        rect.getUpperRight().rotateBy(angle, center_of_rotation),
                        rect.getLowerRight().rotateBy(angle, center_of_rotation),
                        rect.getLowerLeft().rotateBy(angle, center_of_rotation)
                    };

                    if (color == 0xFFFFFFFF)
                    {
                        switch (blend_mode) {
                        case graphics::BlendMode::Set:      graphics::primitives::TexturedQuad(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, vertices, that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_set()); break;
                        case graphics::BlendMode::Mix:      graphics::primitives::TexturedQuad(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, vertices, that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_mix()); break;
                        case graphics::BlendMode::Add:      graphics::primitives::TexturedQuad(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, vertices, that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_add()); break;
                        case graphics::BlendMode::Subtract: graphics::primitives::TexturedQuad(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, vertices, that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_sub()); break;
                        case graphics::BlendMode::Multiply: graphics::primitives::TexturedQuad(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, vertices, that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_mul()); break;
                        }
                    }
                    else
                    {
                        switch (blend_mode) {
                        case graphics::BlendMode::Set:      graphics::primitives::TexturedQuad(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, vertices, that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_set_col(graphics::RGBA8888ToRGBA(color))); break;
                        case graphics::BlendMode::Mix:      graphics::primitives::TexturedQuad(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, vertices, that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_mix_col(graphics::RGBA8888ToRGBA(color))); break;
                        case graphics::BlendMode::Add:      graphics::primitives::TexturedQuad(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, vertices, that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_add_col(graphics::RGBA8888ToRGBA(color))); break;
                        case graphics::BlendMode::Subtract: graphics::primitives::TexturedQuad(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, vertices, that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_sub_col(graphics::RGBA8888ToRGBA(color))); break;
                        case graphics::BlendMode::Multiply: graphics::primitives::TexturedQuad(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, vertices, that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_mul_col(graphics::RGBA8888ToRGBA(color))); break;
                        }
                    }
                }

                return 0;
            }

            //---------------------------------------------------------
            int screen_drawq(lua_State* L)
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

                core::Vec2i vertices[4] = {
                    core::Vec2i(x1, y1),
                    core::Vec2i(x2, y2),
                    core::Vec2i(x3, y3),
                    core::Vec2i(x4, y4)
                };

                if (color == 0xFFFFFFFF)
                {
                    switch (blend_mode) {
                    case graphics::BlendMode::Set:      graphics::primitives::TexturedQuad(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, vertices, that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_set()); break;
                    case graphics::BlendMode::Mix:      graphics::primitives::TexturedQuad(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, vertices, that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_mix()); break;
                    case graphics::BlendMode::Add:      graphics::primitives::TexturedQuad(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, vertices, that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_add()); break;
                    case graphics::BlendMode::Subtract: graphics::primitives::TexturedQuad(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, vertices, that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_sub()); break;
                    case graphics::BlendMode::Multiply: graphics::primitives::TexturedQuad(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, vertices, that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_mul()); break;
                    }
                }
                else
                {
                    switch (blend_mode) {
                    case graphics::BlendMode::Set:      graphics::primitives::TexturedQuad(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, vertices, that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_set_col(graphics::RGBA8888ToRGBA(color))); break;
                    case graphics::BlendMode::Mix:      graphics::primitives::TexturedQuad(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, vertices, that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_mix_col(graphics::RGBA8888ToRGBA(color))); break;
                    case graphics::BlendMode::Add:      graphics::primitives::TexturedQuad(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, vertices, that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_add_col(graphics::RGBA8888ToRGBA(color))); break;
                    case graphics::BlendMode::Subtract: graphics::primitives::TexturedQuad(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, vertices, that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_sub_col(graphics::RGBA8888ToRGBA(color))); break;
                    case graphics::BlendMode::Multiply: graphics::primitives::TexturedQuad(Screen::Pixels(), Screen::Pitch(), Screen::ClipRect, vertices, that->getPixels(), that->getWidth(), core::Recti(sx, sy, sw, sh), rgb565_mul_col(graphics::RGBA8888ToRGBA(color))); break;
                    }
                }

                return 0;
            }

            //---------------------------------------------------------
            int screen_drawText(lua_State* L)
            {
                graphics::Font* font = graphics_module::FontWrapper::Get(L, 1);
                int x                = luaL_checkint(L, 2);
                int y                = luaL_checkint(L, 3);
                size_t len;
                const char* text     = luaL_checklstring(L, 4, &len);
                float scale          = luaL_optnumber(L, 5, 1.0);
                u32 color            = luaL_optint(L, 6, 0xFFFFFFFF);

                int cur_x = x;
                int cur_y = y;

                for (size_t i = 0; i < len; i++)
                {
                    switch (text[i])
                    {
                        case ' ':
                        {
                            const graphics::Image* space_char_image = font->getCharImage(' ');
                            int space_w = (int)(space_char_image ? space_char_image->getWidth() * scale : 0);
                            cur_x += space_w;
                            break;
                        }
                        case '\t':
                        {
                            const graphics::Image* space_char_image = font->getCharImage(' ');
                            int tab_w = (int)(space_char_image ? space_char_image->getWidth() * font->getTabWidth() * scale : 0);
                            if (tab_w > 0) {
                                tab_w = tab_w - ((cur_x - x) % tab_w);
                            }
                            cur_x += tab_w;
                            break;
                        }
                        case '\n':
                        {
                            cur_x = x;
                            cur_y += (int)(font->getMaxCharHeight() * scale);
                            break;
                        }
                        default:
                        {
                            const graphics::Image* char_image = font->getCharImage(text[i]);
                            if (char_image)
                            {
                                if (color == 0xFFFFFFFF)
                                {
                                    graphics::primitives::TexturedRectangle(
                                        Screen::Pixels(),
                                        Screen::Pitch(),
                                        Screen::ClipRect,
                                        core::Recti(cur_x, cur_y, char_image->getWidth(), char_image->getHeight()).scale(scale),
                                        char_image->getPixels(),
                                        char_image->getWidth(),
                                        core::Recti(0, 0, char_image->getWidth(), char_image->getHeight()),
                                        rgb565_mix()
                                    );
                                }
                                else
                                {
                                    graphics::primitives::TexturedRectangle(
                                        Screen::Pixels(),
                                        Screen::Pitch(),
                                        Screen::ClipRect,
                                        core::Recti(cur_x, cur_y, char_image->getWidth(), char_image->getHeight()).scale(scale),
                                        char_image->getPixels(),
                                        char_image->getWidth(),
                                        core::Recti(0, 0, char_image->getWidth(), char_image->getHeight()),
                                        rgb565_mix_col(graphics::RGBA8888ToRGBA(color))
                                    );
                                }
                                cur_x += (int)(char_image->getWidth() * scale);
                            }
                            break;
                        }
                    }
                }

                return 0;
            }

            //---------------------------------------------------------
            int screen_drawWindow(lua_State* L)
            {
                graphics::WindowSkin* windowSkin = graphics_module::WindowSkinWrapper::Get(L, 1);
                int x = luaL_checkint(L, 2);
                int y = luaL_checkint(L, 3);
                int w = luaL_checkint(L, 4);
                int h = luaL_checkint(L, 5);
                int opacity = luaL_optint(L, 6, 255);

                luaL_argcheck(L, w >= 0, 4, "invalid width");
                luaL_argcheck(L, h >= 0, 5, "invalid height");
                luaL_argcheck(L, opacity >= 0 && opacity <= 255, 6, "invalid opacity");

                // for brevity
                int x1 = x;
                int y1 = y;
                int x2 = x + w - 1;
                int y2 = y + h - 1;

                const graphics::Image* tlBorder = windowSkin->getBorderImage(graphics::WindowSkin::TopLeftBorder);
                const graphics::Image* trBorder = windowSkin->getBorderImage(graphics::WindowSkin::TopRightBorder);
                const graphics::Image* brBorder = windowSkin->getBorderImage(graphics::WindowSkin::BottomRightBorder);
                const graphics::Image* blBorder = windowSkin->getBorderImage(graphics::WindowSkin::BottomLeftBorder);

                // draw background
                if (w > 0 && h > 0) {
                    graphics::RGBA tlColor = windowSkin->getBgColor(graphics::WindowSkin::TopLeftBgColor);
                    graphics::RGBA trColor = windowSkin->getBgColor(graphics::WindowSkin::TopRightBgColor);
                    graphics::RGBA brColor = windowSkin->getBgColor(graphics::WindowSkin::BottomRightBgColor);
                    graphics::RGBA blColor = windowSkin->getBgColor(graphics::WindowSkin::BottomLeftBgColor);

                    // apply opacity
                    tlColor.alpha = (tlColor.alpha * opacity) / 255;
                    trColor.alpha = (trColor.alpha * opacity) / 255;
                    brColor.alpha = (brColor.alpha * opacity) / 255;
                    blColor.alpha = (blColor.alpha * opacity) / 255;

                    graphics::primitives::Rectangle(
                        Screen::Pixels(),
                        Screen::Pitch(),
                        Screen::ClipRect,
                        true,
                        core::Recti(x, y, w, h),
                        tlColor,
                        trColor,
                        brColor,
                        blColor,
                        rgb565_mix()
                    );
                }

                // draw top left edge
                graphics::primitives::TexturedRectangle(
                    Screen::Pixels(),
                    Screen::Pitch(),
                    Screen::ClipRect,
                    core::Vec2i(x1 - tlBorder->getWidth(), y1 - tlBorder->getHeight()),
                    tlBorder->getPixels(),
                    tlBorder->getWidth(),
                    core::Recti(tlBorder->getDimensions()),
                    rgb565_mix()
                );

                // draw top right edge
                graphics::primitives::TexturedRectangle(
                    Screen::Pixels(),
                    Screen::Pitch(),
                    Screen::ClipRect,
                    core::Vec2i(x2 + 1, y1 - trBorder->getHeight()),
                    trBorder->getPixels(),
                    trBorder->getWidth(),
                    core::Recti(trBorder->getDimensions()),
                    rgb565_mix()
                );

                // draw bottom right edge
                graphics::primitives::TexturedRectangle(
                    Screen::Pixels(),
                    Screen::Pitch(),
                    Screen::ClipRect,
                    core::Vec2i(x2 + 1, y2 + 1 ),
                    brBorder->getPixels(),
                    brBorder->getWidth(),
                    core::Recti(brBorder->getDimensions()),
                    rgb565_mix()
                );

                // draw bottom left edge
                graphics::primitives::TexturedRectangle(
                    Screen::Pixels(),
                    Screen::Pitch(),
                    Screen::ClipRect,
                    core::Vec2i(x1 - blBorder->getWidth(), y2 + 1 ),
                    blBorder->getPixels(),
                    blBorder->getWidth(),
                    core::Recti(blBorder->getDimensions()),
                    rgb565_mix()
                );

                // draw top and bottom borders
                if (w > 0)
                {
                    const graphics::Image* tBorder  = windowSkin->getBorderImage(graphics::WindowSkin::TopBorder);
                    const graphics::Image* bBorder  = windowSkin->getBorderImage(graphics::WindowSkin::BottomBorder);

                    int i;

                    // draw top border
                    i = x1;
                    while ((x2 - i) + 1 >= tBorder->getWidth()) {
                        graphics::primitives::TexturedRectangle(
                            Screen::Pixels(),
                            Screen::Pitch(),
                            Screen::ClipRect,
                            core::Vec2i(i, y1 - tBorder->getHeight()),
                            tBorder->getPixels(),
                            tBorder->getWidth(),
                            core::Recti(tBorder->getDimensions()),
                            rgb565_mix()
                        );

                        i += tBorder->getWidth();
                    }

                    if ((x2 - i) + 1 > 0) {
                        graphics::primitives::TexturedRectangle(
                            Screen::Pixels(),
                            Screen::Pitch(),
                            Screen::ClipRect,
                            core::Vec2i(i, y1 - tBorder->getHeight()),
                            tBorder->getPixels(),
                            tBorder->getWidth(),
                            core::Recti(0, 0, (x2 - i) + 1, tBorder->getHeight()),
                            rgb565_mix()
                        );
                    }

                    // draw bottom border
                    i = x1;
                    while ((x2 - i) + 1 >= bBorder->getWidth()) {
                        graphics::primitives::TexturedRectangle(
                            Screen::Pixels(),
                            Screen::Pitch(),
                            Screen::ClipRect,
                            core::Vec2i(i, y2 + 1),
                            bBorder->getPixels(),
                            bBorder->getWidth(),
                            core::Recti(bBorder->getDimensions()),
                            rgb565_mix()
                        );

                        i += bBorder->getWidth();
                    }

                    if ((x2 - i) + 1 > 0) {
                        graphics::primitives::TexturedRectangle(
                            Screen::Pixels(),
                            Screen::Pitch(),
                            Screen::ClipRect,
                            core::Vec2i(i, y2 + 1),
                            bBorder->getPixels(),
                            bBorder->getWidth(),
                            core::Recti(0, 0, (x2 - i) + 1, bBorder->getHeight()),
                            rgb565_mix()
                        );
                    }
                }

                // draw left and right borders
                if (h > 0)
                {
                    const graphics::Image* lBorder  = windowSkin->getBorderImage(graphics::WindowSkin::LeftBorder);
                    const graphics::Image* rBorder  = windowSkin->getBorderImage(graphics::WindowSkin::RightBorder);

                    int i;

                    // draw left border
                    i = y1;
                    while ((y2 - i) + 1 >= lBorder->getHeight()) {
                        graphics::primitives::TexturedRectangle(
                            Screen::Pixels(),
                            Screen::Pitch(),
                            Screen::ClipRect,
                            core::Vec2i(x1 - lBorder->getWidth(), i),
                            lBorder->getPixels(),
                            lBorder->getWidth(),
                            core::Recti(lBorder->getDimensions()),
                            rgb565_mix()
                        );

                        i += lBorder->getHeight();
                    }

                    if ((y2 - i) + 1 > 0) {
                        graphics::primitives::TexturedRectangle(
                            Screen::Pixels(),
                            Screen::Pitch(),
                            Screen::ClipRect,
                            core::Vec2i(x1 - lBorder->getWidth(), i),
                            lBorder->getPixels(),
                            lBorder->getWidth(),
                            core::Recti(0, 0, lBorder->getWidth(), (y2 - i) + 1),
                            rgb565_mix()
                        );
                    }

                    // draw right border
                    i = y1;
                    while ((y2 - i) + 1 >= rBorder->getHeight()) {
                        graphics::primitives::TexturedRectangle(
                            Screen::Pixels(),
                            Screen::Pitch(),
                            Screen::ClipRect,
                            core::Vec2i(x2 + 1, i),
                            rBorder->getPixels(),
                            rBorder->getWidth(),
                            core::Recti(rBorder->getDimensions()),
                            rgb565_mix()
                        );

                        i += rBorder->getHeight();
                    }

                    if ((y2 - i) + 1 > 0) {
                        graphics::primitives::TexturedRectangle(
                            Screen::Pixels(),
                            Screen::Pitch(),
                            Screen::ClipRect,
                            core::Vec2i(x2 + 1, i),
                            rBorder->getPixels(),
                            rBorder->getWidth(),
                            core::Recti(0, 0, rBorder->getWidth(), (y2 - i) + 1),
                            rgb565_mix()
                        );
                    }
                }

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
            int game_variables_proxy_table_indexMetaMethod(lua_State* L)
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
            int game_variables_proxy_table_newindexMetaMethod(lua_State* L)
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
            int game_variables_proxy_table_lenMetaMethod(lua_State* L)
            {
                lua_pushnumber(L, RPG::system->variables.size);
                return 1;
            }

            //---------------------------------------------------------
            int game_variables_proxy_table_resize(lua_State* L)
            {
                int n = luaL_checkint(L, 1);
                luaL_argcheck(L, n >= 0 && n <= RPGSS_SANE_VARIABLE_ARRAY_SIZE_LIMIT, 2, "invalid size");
                RPG::system->variables.resize(n);
                return 0;
            }

            //---------------------------------------------------------
            luabridge::LuaRef game_get_variables()
            {
                lua_State* L = Context::Current().interpreter();

                lua_pushstring(L, "__game_variables_proxy_table");
                lua_rawget(L, LUA_REGISTRYINDEX); // get proxy table from registry table

                if (!lua_istable(L, -1)) {
                    lua_newtable(L); // proxy table

                    lua_pushstring(L, "resize");
                    lua_pushcfunction(L, game_variables_proxy_table_resize);
                    lua_rawset(L, -3); // add resize() to proxy table

                    lua_newtable(L); // metatable

                    lua_pushstring(L, "__index");
                    lua_pushcfunction(L, game_variables_proxy_table_indexMetaMethod);
                    lua_rawset(L, -3); // add __index() to metatable

                    lua_pushstring(L, "__newindex");
                    lua_pushcfunction(L, game_variables_proxy_table_newindexMetaMethod);
                    lua_rawset(L, -3); // add __newindex() to metatable

                    lua_pushstring(L, "__len");
                    lua_pushcfunction(L, game_variables_proxy_table_lenMetaMethod);
                    lua_rawset(L, -3); // add __len() to metatable

                    lua_setmetatable(L, -2); // set metatable for proxy table

                    lua_pushstring(L, "__game_variables_proxy_table");
                    lua_pushvalue(L, -2); // push proxy table
                    lua_rawset(L, LUA_REGISTRYINDEX); // store proxy table in registry table

                    assert(lua_istable(L, -1));
                }

                return luabridge::LuaRef::fromStack(L, -1);
            }

            //---------------------------------------------------------
            int game_switches_proxy_table_indexMetaMethod(lua_State* L)
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
            int game_switches_proxy_table_newindexMetaMethod(lua_State* L)
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
            int game_switches_proxy_table_lenMetaMethod(lua_State* L)
            {
                lua_pushnumber(L, RPG::system->switches.size);
                return 1;
            }

            //---------------------------------------------------------
            int game_switches_proxy_table_resize(lua_State* L)
            {
                int n = luaL_checkint(L, 1);
                luaL_argcheck(L, n >= 0 && n <= RPGSS_SANE_SWITCH_ARRAY_SIZE_LIMIT, 2, "invalid size");
                RPG::system->switches.resize(n);
                return 0;
            }

            //---------------------------------------------------------
            luabridge::LuaRef game_get_switches()
            {
                lua_State* L = Context::Current().interpreter();

                lua_pushstring(L, "__game_switches_proxy_table");
                lua_rawget(L, LUA_REGISTRYINDEX); // get proxy table from registry table

                if (!lua_istable(L, -1)) {
                    lua_newtable(L); // proxy table

                    lua_pushstring(L, "resize");
                    lua_pushcfunction(L, game_switches_proxy_table_resize);
                    lua_rawset(L, -3); // add resize() to proxy table

                    lua_newtable(L); // metatable

                    lua_pushstring(L, "__index");
                    lua_pushcfunction(L, game_switches_proxy_table_indexMetaMethod);
                    lua_rawset(L, -3); // add __index() to metatable

                    lua_pushstring(L, "__newindex");
                    lua_pushcfunction(L, game_switches_proxy_table_newindexMetaMethod);
                    lua_rawset(L, -3); // add __newindex() to metatable

                    lua_pushstring(L, "__len");
                    lua_pushcfunction(L, game_switches_proxy_table_lenMetaMethod);
                    lua_rawset(L, -3); // add __len() to metatable

                    lua_setmetatable(L, -2); // set metatable for proxy table

                    lua_pushstring(L, "__game_switches_proxy_table");
                    lua_pushvalue(L, -2); // push proxy table
                    lua_rawset(L, LUA_REGISTRYINDEX); // store proxy table in registry table

                    assert(lua_istable(L, -1));
                }

                return luabridge::LuaRef::fromStack(L, -1);
            }

            //---------------------------------------------------------
            int game_actors_proxy_table_indexMetaMethod(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                if (n < 1 || n > RPG::actors.count()) {
                   return luaL_error(L, "index out of bounds");
                }
                luabridge::push(L, ActorWrapper(RPG::actors[n]));
                return 1;
            }

            //---------------------------------------------------------
            int game_actors_proxy_table_newindexMetaMethod(lua_State* L)
            {
               return luaL_error(L, "operation not allowed");
            }

            //---------------------------------------------------------
            int game_actors_proxy_table_lenMetaMethod(lua_State* L)
            {
                lua_pushnumber(L, RPG::actors.count());
                return 1;
            }

            //---------------------------------------------------------
            luabridge::LuaRef game_get_actors()
            {
                lua_State* L = Context::Current().interpreter();

                lua_pushstring(L, "__game_actors_proxy_table");
                lua_rawget(L, LUA_REGISTRYINDEX); // get proxy table from registry table

                if (!lua_istable(L, -1)) {
                    lua_newtable(L); // proxy table

                    lua_newtable(L); // metatable

                    lua_pushstring(L, "__index");
                    lua_pushcfunction(L, game_actors_proxy_table_indexMetaMethod);
                    lua_rawset(L, -3); // add __index() to metatable

                    lua_pushstring(L, "__newindex");
                    lua_pushcfunction(L, game_actors_proxy_table_newindexMetaMethod);
                    lua_rawset(L, -3); // add __newindex() to metatable

                    lua_pushstring(L, "__len");
                    lua_pushcfunction(L, game_actors_proxy_table_lenMetaMethod);
                    lua_rawset(L, -3); // add __len() to metatable

                    lua_setmetatable(L, -2); // set metatable for proxy table

                    lua_pushstring(L, "__game_actors_proxy_table");
                    lua_pushvalue(L, -2); // push proxy table
                    lua_rawset(L, LUA_REGISTRYINDEX); // store proxy table in registry table

                    assert(lua_istable(L, -1));
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
            int game_actor_party_proxy_table_indexMetaMethod(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                if (n < 1 || n > game_get_actor_party_size()) {
                   return luaL_error(L, "index out of bounds");
                }
                luabridge::push(L, ActorWrapper(RPG::Actor::partyMember(n-1)));
                return 1;
            }

            //---------------------------------------------------------
            int game_actor_party_proxy_table_newindexMetaMethod(lua_State* L)
            {
               return luaL_error(L, "operation not allowed");
            }

            //---------------------------------------------------------
            int game_actor_party_proxy_table_lenMetaMethod(lua_State* L)
            {
                lua_pushnumber(L, game_get_actor_party_size());
                return 1;
            }

            //---------------------------------------------------------
            luabridge::LuaRef game_get_actorParty()
            {
                lua_State* L = Context::Current().interpreter();

                lua_pushstring(L, "__game_actor_party_proxy_table");
                lua_rawget(L, LUA_REGISTRYINDEX); // get proxy table from registry table

                if (!lua_istable(L, -1)) {
                    lua_newtable(L); // proxy table

                    lua_newtable(L); // metatable

                    lua_pushstring(L, "__index");
                    lua_pushcfunction(L, game_actor_party_proxy_table_indexMetaMethod);
                    lua_rawset(L, -3); // add __index() to metatable

                    lua_pushstring(L, "__newindex");
                    lua_pushcfunction(L, game_actor_party_proxy_table_newindexMetaMethod);
                    lua_rawset(L, -3); // add __newindex() to metatable

                    lua_pushstring(L, "__len");
                    lua_pushcfunction(L, game_actor_party_proxy_table_lenMetaMethod);
                    lua_rawset(L, -3); // add __len() to metatable

                    lua_setmetatable(L, -2); // set metatable for proxy table

                    lua_pushstring(L, "__game_actor_party_proxy_table");
                    lua_pushvalue(L, -2); // push proxy table
                    lua_rawset(L, LUA_REGISTRYINDEX); // store proxy table in registry table

                    assert(lua_istable(L, -1));
                }

                return luabridge::LuaRef::fromStack(L, -1);
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

                        .addProperty("scene",               &game_get_scene)
                        .addProperty("isTestPlay",          &game_get_isTestPlay)
                        .addProperty("isBattleTest",        &game_get_isBattleTest)
                        .addProperty("saveCount",           &game_get_saveCount)
                        .addProperty("variables",           &game_get_variables)
                        .addProperty("switches",            &game_get_switches)
                        .addProperty("actors",              &game_get_actors)
                        .addProperty("party",               &game_get_actorParty)

                        .beginNamespace("map")
                            .addProperty("hero",                    &map_get_hero)
                            .addProperty("events",                  &map_get_events)
                            .addProperty("id",                      &map_get_id)
                            .addProperty("width",                   &map_get_width)
                            .addProperty("height",                  &map_get_height)
                            .addProperty("cameraX",                 &map_get_cameraX)
                            .addProperty("cameraY",                 &map_get_cameraY)
                            .addCFunction("findEventById",          &map_findEventById)
                            .addCFunction("findEventByName",        &map_findEventByName)
                            .addCFunction("updateEvents",           &map_updateEvents)
                            .addCFunction("getCameraPosition",      &map_getCameraPosition)
                            .addCFunction("setCameraPosition",      &map_setCameraPosition)
                            .addCFunction("moveCamera",             &map_moveCamera)
                            .addCFunction("getLowerLayerTileId",    &map_getLowerLayerTileId)
                            .addCFunction("getUpperLayerTileId",    &map_getUpperLayerTileId)
                            .addCFunction("getTerrainId",           &map_getTerrainId)
                        .endNamespace()

                        .beginNamespace("battle")
                            .addProperty("enemies", &battle_get_monsterParty)
                        .endNamespace()

                        .beginNamespace("input")
                            .addCFunction("pressed", &input_pressed)
                            .addCFunction("assign",  &input_assign)
                        .endNamespace()

                        .beginNamespace("music")
                            .addCFunction("play",    &music_play)
                            .addCFunction("fadeOut", &music_fadeOut)
                            .addCFunction("stop",    &music_stop)
                        .endNamespace()

                        .beginNamespace("sound")
                            .addCFunction("play", &sound_play)
                            .addCFunction("stop", &sound_stop)
                        .endNamespace()

                        .beginNamespace("screen")
                            .addProperty("width",                   &screen_get_width)
                            .addProperty("height",                  &screen_get_height)
                            .addProperty("brightness",              &screen_get_brightness, &screen_set_brightness)
                            .addCFunction("getDimensions",          &screen_getDimensions)
                            .addCFunction("getClipRect",            &screen_getClipRect)
                            .addCFunction("setClipRect",            &screen_setClipRect)
                            .addCFunction("copyRect",               &screen_copyRect)
                            .addCFunction("clear",                  &screen_clear)
                            .addCFunction("grey",                   &screen_grey)
                            .addCFunction("getPixel",               &screen_getPixel)
                            .addCFunction("setPixel",               &screen_setPixel)
                            .addCFunction("drawPoint",              &screen_drawPoint)
                            .addCFunction("drawLine",               &screen_drawLine)
                            .addCFunction("drawRectangle",          &screen_drawRectangle)
                            .addCFunction("drawCircle",             &screen_drawCircle)
                            .addCFunction("drawTriangle",           &screen_drawTriangle)
                            .addCFunction("draw",                   &screen_draw)
                            .addCFunction("drawq",                  &screen_drawq)
                            .addCFunction("drawText",               &screen_drawText)
                            .addCFunction("drawWindow",             &screen_drawWindow)
                        .endNamespace()

                    .endNamespace();

                return true;
            }

        } // game_module
    } // namespace script
} // namespace rpgss
