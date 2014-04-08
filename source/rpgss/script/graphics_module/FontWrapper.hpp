#ifndef RPGSS_SCRIPT_GRAPHICS_MODULE_FONTWRAPPER_HPP_INCLUDED
#define RPGSS_SCRIPT_GRAPHICS_MODULE_FONTWRAPPER_HPP_INCLUDED

#include "../../graphics/Font.hpp"
#include "../lua_include.hpp"


namespace rpgss {
    namespace script {
        namespace graphics_module {

            class FontWrapper {
            public:
                static void Push(lua_State* L, graphics::Font* font);
                static bool Is(lua_State* L, int index);
                static graphics::Font* Get(lua_State* L, int index);
                static graphics::Font* GetOpt(lua_State* L, int index);

                explicit FontWrapper(graphics::Font* ptr);

                int get_maxCharWidth() const;
                int get_maxCharHeight() const;
                int get_tabWidth() const;
                void set_tabWidth(int tabWidth);
                int getStringWidth(lua_State* L);
                int wordWrapString(lua_State* L);

            private:
                graphics::Font::Ptr This;
            };

        } // namespace graphics_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_GRAPHICS_MODULE_FONTWRAPPER_HPP_INCLUDED
