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

#include <cassert>

#define NOT_MAIN_MODULE
#include <DynRPG/DynRPG.h>

#include "../../Context.hpp"
#include "constants.hpp"
#include "CharacterWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace game_module {

            //---------------------------------------------------------
            void
            CharacterWrapper::Push(lua_State* L, RPG::Character* character)
            {
                assert(character);
                luabridge::push(L, CharacterWrapper(character));
            }

            //---------------------------------------------------------
            bool
            CharacterWrapper::Is(lua_State* L, int index)
            {
                assert(index != 0);

                if (index < 0) { // allow negative indices
                    index = lua_gettop(L) + index + 1;
                }

                return luabridge::Stack<CharacterWrapper*>::is_a(L, index);
            }

            //---------------------------------------------------------
            RPG::Character*
            CharacterWrapper::Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "Character expected, got nothing");
                    return 0;
                }
                CharacterWrapper* wrapper = luabridge::Stack<CharacterWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "Character expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            RPG::Character*
            CharacterWrapper::GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                CharacterWrapper* wrapper = luabridge::Stack<CharacterWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            CharacterWrapper::CharacterWrapper(RPG::Character* ptr)
                : This(ptr)
            {
            }

            //---------------------------------------------------------
            std::string
            CharacterWrapper::get_name() const
            {
                return const_cast<RPG::Character*>(This)->getName();
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::get_id() const
            {
                return const_cast<RPG::Character*>(This)->id;
            }

            //---------------------------------------------------------
            std::string
            CharacterWrapper::get_charsetFilename() const
            {
                return const_cast<RPG::Character*>(This)->charsetFilename.s_str();
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::get_charsetId() const
            {
                return const_cast<RPG::Character*>(This)->charsetId;
            }

            //---------------------------------------------------------
            bool
            CharacterWrapper::get_enabled() const
            {
                return const_cast<RPG::Character*>(This)->enabled;
            }

            //---------------------------------------------------------
            void
            CharacterWrapper::set_enabled(bool enabled)
            {
                This->enabled = enabled;
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::get_screenX() const
            {
                return const_cast<RPG::Character*>(This)->getScreenX();
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::get_screenY() const
            {
                return const_cast<RPG::Character*>(This)->getScreenY();
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::get_x() const
            {
                return const_cast<RPG::Character*>(This)->x;
            }

            //---------------------------------------------------------
            void
            CharacterWrapper::set_x(int x)
            {
                This->x = x;
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::get_y() const
            {
                return const_cast<RPG::Character*>(This)->y;
            }

            //---------------------------------------------------------
            void
            CharacterWrapper::set_y(int y)
            {
                This->y = y;
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::get_offsetX() const
            {
                return const_cast<RPG::Character*>(This)->offsetX;
            }

            //---------------------------------------------------------
            void
            CharacterWrapper::set_offsetX(int offsetX)
            {
                This->offsetX = offsetX;
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::get_offsetY() const
            {
                return const_cast<RPG::Character*>(This)->offsetY;
            }

            //---------------------------------------------------------
            void
            CharacterWrapper::set_offsetY(int offsetY)
            {
                This->offsetY = offsetY;
            }

            //---------------------------------------------------------
            std::string
            CharacterWrapper::get_direction() const
            {
                std::string direction_str;
                if (!GetDirectionConstant(This->direction, direction_str)) {
                    luaL_error(Context::Current().interpreter(), "unexpected internal value");
                }
                return direction_str;
            }

            //---------------------------------------------------------
            void
            CharacterWrapper::set_direction(const std::string& direction_str)
            {
                int direction;
                if (!GetDirectionConstant(direction_str, direction)) {
                    luaL_error(Context::Current().interpreter(), "invalid value");
                }
                This->direction = direction;
            }

            //---------------------------------------------------------
            std::string
            CharacterWrapper::get_facing() const
            {
                std::string facing_str;
                if (!GetFacingConstant(This->facing, facing_str)) {
                    luaL_error(Context::Current().interpreter(), "unexpected internal value");
                }
                return facing_str;
            }

            //---------------------------------------------------------
            void
            CharacterWrapper::set_facing(const std::string& facing_str)
            {
                int facing;
                if (!GetFacingConstant(facing_str, facing)) {
                    luaL_error(Context::Current().interpreter(), "invalid value");
                }
                This->facing = facing;
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::get_step() const
            {
                return const_cast<RPG::Character*>(This)->step;
            }

            //---------------------------------------------------------
            void
            CharacterWrapper::set_step(int step)
            {
                if (step < 0 || step > 3) {
                    luaL_error(Context::Current().interpreter(), "invalid value");
                }
                This->step = step;
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::get_transparency() const
            {
                return const_cast<RPG::Character*>(This)->transparency;
            }

            //---------------------------------------------------------
            void
            CharacterWrapper::set_transparency(int transparency)
            {
                if (transparency < 0 || transparency > 8) {
                    luaL_error(Context::Current().interpreter(), "invalid value");
                }
                This->transparency = transparency;
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::get_frequency() const
            {
                return const_cast<RPG::Character*>(This)->frequency;
            }

            //---------------------------------------------------------
            void
            CharacterWrapper::set_frequency(int frequency)
            {
                if (frequency < 1 || frequency > 8) {
                    luaL_error(Context::Current().interpreter(), "invalid value");
                }
                This->frequency = frequency;
            }

            //---------------------------------------------------------
            std::string
            CharacterWrapper::get_layer() const
            {
                std::string layer_str;
                if (!GetLayerConstant(This->layer, layer_str)) {
                    luaL_error(Context::Current().interpreter(), "unexpected internal value");
                }
                return layer_str;
            }

            //---------------------------------------------------------
            void
            CharacterWrapper::set_layer(const std::string& layer_str)
            {
                int layer;
                if (!GetLayerConstant(layer_str, layer)) {
                    luaL_error(Context::Current().interpreter(), "invalid value");
                }
                This->layer = layer;
            }

            //---------------------------------------------------------
            bool
            CharacterWrapper::get_forbidEventOverlap() const
            {
                return const_cast<RPG::Character*>(This)->forbidEventOverlap;
            }

            //---------------------------------------------------------
            void
            CharacterWrapper::set_forbidEventOverlap(bool forbidEventOverlap)
            {
                This->forbidEventOverlap = forbidEventOverlap;
            }

            //---------------------------------------------------------
            std::string
            CharacterWrapper::get_animationType() const
            {
                std::string animationType_str;
                if (!GetAnimationTypeConstant(This->animationType, animationType_str)) {
                    luaL_error(Context::Current().interpreter(), "unexpected internal value");
                }
                return animationType_str;
            }

            //---------------------------------------------------------
            void
            CharacterWrapper::set_animationType(const std::string& animationType_str)
            {
                int animationType;
                if (!GetAnimationTypeConstant(animationType_str, animationType)) {
                    luaL_error(Context::Current().interpreter(), "invalid value");
                }
                This->animationType = animationType;
            }

            //---------------------------------------------------------
            bool
            CharacterWrapper::get_fixedDirection() const
            {
                return const_cast<RPG::Character*>(This)->fixedDirection;
            }

            //---------------------------------------------------------
            void
            CharacterWrapper::set_fixedDirection(bool fixedDirection)
            {
                This->fixedDirection = fixedDirection;
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::get_speed() const
            {
                return const_cast<RPG::Character*>(This)->speed;
            }

            //---------------------------------------------------------
            void
            CharacterWrapper::set_speed(int speed)
            {
                if (speed < 1 || speed > 6) {
                    luaL_error(Context::Current().interpreter(), "invalid value");
                }
                This->speed = speed;
            }

            //---------------------------------------------------------
            bool
            CharacterWrapper::get_moving() const
            {
                return const_cast<RPG::Character*>(This)->moving;
            }

            //---------------------------------------------------------
            void
            CharacterWrapper::set_moving(bool moving)
            {
                This->moving = moving;
            }

            //---------------------------------------------------------
            bool
            CharacterWrapper::get_phasing() const
            {
                return const_cast<RPG::Character*>(This)->phasing;
            }

            //---------------------------------------------------------
            void
            CharacterWrapper::set_phasing(bool phasing)
            {
                This->phasing = phasing;
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::getScreenPosition(lua_State* L)
            {
                lua_pushnumber(L, This->getScreenX());
                lua_pushnumber(L, This->getScreenY());
                return 2;
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::getPosition(lua_State* L)
            {
                lua_pushnumber(L, This->x);
                lua_pushnumber(L, This->y);
                return 2;
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::setPosition(lua_State* L)
            {
                int x = luaL_checkint(L, 2);
                int y = luaL_checkint(L, 3);
                This->x = x;
                This->y = y;
                return 0;
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::getPositionOffset(lua_State* L)
            {
                lua_pushnumber(L, This->offsetX);
                lua_pushnumber(L, This->offsetY);
                return 2;
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::setPositionOffset(lua_State* L)
            {
                int offsetX = luaL_checkint(L, 2);
                int offsetY = luaL_checkint(L, 3);
                This->offsetX = offsetX;
                This->offsetY = offsetY;
                return 0;
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::setFixedStep(lua_State* L)
            {
                int newStep = luaL_checkint(L, 2);
                if (newStep < 0 || newStep > 3) {
                    return luaL_argerror(L, 2, "invalid step value");
                }
                This->setFixedStep(newStep);
                return 0;
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::setNormalStep(lua_State* L)
            {
                This->setNormalStep();
                return 0;
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::isMovePossible(lua_State* L)
            {
                int fromX = luaL_checkint(L, 2);
                int fromY = luaL_checkint(L, 3);
                int toX   = luaL_checkint(L, 4);
                int toY   = luaL_checkint(L, 5);
                lua_pushboolean(L, This->isMovePossible(fromX, fromY, toX, toY));
                return 1;
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::stop(lua_State* L)
            {
                This->stop();
                return 0;
            }

            //---------------------------------------------------------
            int
            CharacterWrapper::doStep(lua_State* L)
            {
                const char* direction_str = luaL_checkstring(L, 2);
                int direction;
                if (!GetDirectionConstant(direction_str, direction)) {
                    return luaL_argerror(L, 2, "invalid direction constant");
                }
                This->doStep((RPG::Direction)direction);
                return 0;
            }

        } // namespace game_module
    } // namespace script
} // namespace rpgss
