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
                int getTextWidth(lua_State* L);
                int wordWrapText(lua_State* L);

            private:
                graphics::Font::Ptr This;
            };

        } // namespace graphics_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_GRAPHICS_MODULE_FONTWRAPPER_HPP_INCLUDED
