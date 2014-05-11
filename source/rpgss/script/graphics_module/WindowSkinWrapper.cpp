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

#include "../../Context.hpp"
#include "WindowSkinWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace graphics_module {

            //---------------------------------------------------------
            void
            WindowSkinWrapper::Push(lua_State* L, graphics::WindowSkin* windowskin)
            {
                assert(windowskin);
                luabridge::push(L, WindowSkinWrapper(windowskin));
            }

            //---------------------------------------------------------
            bool
            WindowSkinWrapper::Is(lua_State* L, int index)
            {
                assert(index != 0);

                if (index < 0) { // allow negative indices
                    index = lua_gettop(L) + index + 1;
                }

                return luabridge::Stack<WindowSkinWrapper*>::is_a(L, index);
            }

            //---------------------------------------------------------
            graphics::WindowSkin*
            WindowSkinWrapper::Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "WindowSkin expected, got nothing");
                    return 0;
                }
                WindowSkinWrapper* wrapper = luabridge::Stack<WindowSkinWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "WindowSkin expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            graphics::WindowSkin*
            WindowSkinWrapper::GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                WindowSkinWrapper* wrapper = luabridge::Stack<WindowSkinWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            WindowSkinWrapper::WindowSkinWrapper(graphics::WindowSkin* ptr)
                : This(ptr)
            {
            }

            //---------------------------------------------------------
            u32
            WindowSkinWrapper::get_topLeftColor() const
            {
                return This->getBgColor(graphics::WindowSkin::TopLeftBgColor).toRGBA8888();
            }

            //---------------------------------------------------------
            void
            WindowSkinWrapper::set_topLeftColor(u32 newColor)
            {
                This->setBgColor(graphics::WindowSkin::TopLeftBgColor, graphics::RGBA8888ToRGBA(newColor));
            }

            //---------------------------------------------------------
            u32
            WindowSkinWrapper::get_topRightColor() const
            {
                return This->getBgColor(graphics::WindowSkin::TopRightBgColor).toRGBA8888();
            }

            //---------------------------------------------------------
            void
            WindowSkinWrapper::set_topRightColor(u32 newColor)
            {
                This->setBgColor(graphics::WindowSkin::TopRightBgColor, graphics::RGBA8888ToRGBA(newColor));
            }

            //---------------------------------------------------------
            u32
            WindowSkinWrapper::get_bottomRightColor() const
            {
                return This->getBgColor(graphics::WindowSkin::BottomRightBgColor).toRGBA8888();
            }

            //---------------------------------------------------------
            void
            WindowSkinWrapper::set_bottomRightColor(u32 newColor)
            {
                This->setBgColor(graphics::WindowSkin::BottomRightBgColor, graphics::RGBA8888ToRGBA(newColor));
            }

            //---------------------------------------------------------
            u32
            WindowSkinWrapper::get_bottomLeftColor() const
            {
                return This->getBgColor(graphics::WindowSkin::BottomLeftBgColor).toRGBA8888();
            }

            //---------------------------------------------------------
            void
            WindowSkinWrapper::set_bottomLeftColor(u32 newColor)
            {
                This->setBgColor(graphics::WindowSkin::BottomLeftBgColor, graphics::RGBA8888ToRGBA(newColor));
            }

        } // namespace graphics_module
    } // namespace script
} // namespace rpgss
