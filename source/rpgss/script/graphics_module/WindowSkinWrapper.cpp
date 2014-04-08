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
