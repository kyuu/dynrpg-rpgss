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

#ifndef RPGSS_GRAPHICS_RGBA_HPP_INCLUDED
#define RPGSS_GRAPHICS_RGBA_HPP_INCLUDED

#include "../common/types.hpp"


namespace rpgss {
    namespace graphics {

        struct RGBA {
            u8 red;
            u8 green;
            u8 blue;
            u8 alpha;

            RGBA() : red(0), green(0), blue(0), alpha(255) { }

            RGBA(u8 r, u8 g, u8 b, u8 a = 255) : red(r), green(g), blue(b), alpha(a) { }

            RGBA(const RGBA& that) : red(that.red), green(that.green), blue(that.blue), alpha(that.alpha) { }

            bool operator==(const RGBA& rhs) const {
                return (red   == rhs.red   &&
                        green == rhs.green &&
                        blue  == rhs.blue  &&
                        alpha == rhs.alpha);
            }

            bool operator!=(const RGBA& rhs) const {
                return !(*this == rhs);
            }

            u32 toRGBA8888() const {
                return (
                    (red   << 24) |
                    (green << 16) |
                    (blue  <<  8) |
                     alpha
                );
            }

            u16 toRGB565() const {
                return (
                    ((red   & 0xF8) << 8) |
                    ((green & 0xFC) << 3) |
                     (blue          >> 3)
                );
            }
        };

        inline u32 RGBA8888(u8 r, u8 g, u8 b, u8 a = 255)
        {
            return (
                (r << 24) |
                (g << 16) |
                (b <<  8) |
                 a
            );
        }

        inline u16 RGB565(u8 r, u8 g, u8 b)
        {
            return (
                ((r & 0xF8) << 8) |
                ((g & 0xFC) << 3) |
                 (b         >> 3)
            );
        }

        inline RGBA RGBA8888ToRGBA(u32 c)
        {
            return RGBA(
                c >> 24,
                c >> 16,
                c >>  8,
                c
            );
        }

        inline RGBA RGB565ToRGBA(u16 c)
        {
            return RGBA(
                (c >> 8) | 0x07,
                (c >> 3) | 0x03,
                (c << 3) | 0x07
            );
        }

        inline u32 RGBAToRGBA8888(RGBA c)
        {
            return (
                (c.red   << 24) |
                (c.green << 16) |
                (c.blue  <<  8) |
                 c.alpha
            );
        }

        inline u32 RGB565ToRGBA8888(u16 c)
        {
            return (
                ((c & 0xF800) << 16) |
                ((c & 0x07E0) << 13) |
                ((c & 0x001F) << 11) |
                0x070307FF
            );
        }

        inline u16 RGBAToRGB565(RGBA c)
        {
            return (
                ((c.red   & 0xF8) << 8) |
                ((c.green & 0xFC) << 3) |
                 (c.blue          >> 3)
            );
        }

        inline u16 RGBA8888ToRGB565(u32 c)
        {
            return (
                ((c & 0xF8000000) >> 16) |
                ((c & 0x00FC0000) >> 13) |
                ((c & 0x0000F800) >> 11)
            );
        }

    } // namespace graphics
} // namespace rpgss


#endif // RPGSS_GRAPHICS_RGBA_HPP_INCLUDED
