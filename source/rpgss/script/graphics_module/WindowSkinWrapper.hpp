#ifndef RPGSS_SCRIPT_GRAPHICS_MODULE_WINDOWSKINWRAPPER_HPP_INCLUDED
#define RPGSS_SCRIPT_GRAPHICS_MODULE_WINDOWSKINWRAPPER_HPP_INCLUDED

#include "../../graphics/WindowSkin.hpp"
#include "../lua_include.hpp"


namespace rpgss {
    namespace script {
        namespace graphics_module {

            class WindowSkinWrapper {
            public:
                static void Push(lua_State* L, graphics::WindowSkin* windowskin);
                static bool Is(lua_State* L, int index);
                static graphics::WindowSkin* Get(lua_State* L, int index);
                static graphics::WindowSkin* GetOpt(lua_State* L, int index);

                explicit WindowSkinWrapper(graphics::WindowSkin* ptr);

                u32 get_topLeftColor() const;
                void set_topLeftColor(u32 newColor);
                u32 get_topRightColor() const;
                void set_topRightColor(u32 newColor);
                u32 get_bottomRightColor() const;
                void set_bottomRightColor(u32 newColor);
                u32 get_bottomLeftColor() const;
                void set_bottomLeftColor(u32 newColor);

            private:
                graphics::WindowSkin::Ptr This;
            };

        } // namespace graphics_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_GRAPHICS_MODULE_WINDOWSKINWRAPPER_HPP_INCLUDED
