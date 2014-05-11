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

#ifndef RPGSS_SCRIPT_GRAPHICS_MODULE_IMAGEWRAPPER_HPP_INCLUDED
#define RPGSS_SCRIPT_GRAPHICS_MODULE_IMAGEWRAPPER_HPP_INCLUDED

#include "../../graphics/Image.hpp"
#include "../lua_include.hpp"


namespace rpgss {
    namespace script {
        namespace graphics_module {

            class ImageWrapper {
            public:
                static void Push(lua_State* L, graphics::Image* image);
                static bool Is(lua_State* L, int index);
                static graphics::Image* Get(lua_State* L, int index);
                static graphics::Image* GetOpt(lua_State* L, int index);

                explicit ImageWrapper(graphics::Image* ptr);

                int get_width() const;
                int get_height() const;
                int __len(lua_State* L);
                int getDimensions(lua_State* L);
                int getBlendMode(lua_State* L);
                int setBlendMode(lua_State* L);
                int getClipRect(lua_State* L);
                int setClipRect(lua_State* L);
                int copyPixels(lua_State* L);
                int copyRect(lua_State* L);
                int resize(lua_State* L);
                int setAlpha(lua_State* L);
                int clear(lua_State* L);
                int grey(lua_State* L);
                int flip(lua_State* L);
                int rotate(lua_State* L);
                int getPixel(lua_State* L);
                int setPixel(lua_State* L);
                int drawPoint(lua_State* L);
                int drawLine(lua_State* L);
                int drawRectangle(lua_State* L);
                int drawCircle(lua_State* L);
                int drawTriangle(lua_State* L);
                int draw(lua_State* L);
                int drawq(lua_State* L);
                int drawText(lua_State* L);
                int drawWindow(lua_State* L);

            private:
                graphics::Image::Ptr This;
            };

        } // namespace graphics_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_GRAPHICS_MODULE_IMAGEWRAPPER_HPP_INCLUDED
