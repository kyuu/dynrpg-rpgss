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

#ifndef RPGSS_GRAPHICS_WINDOWSKIN_HPP_INCLUDED
#define RPGSS_GRAPHICS_WINDOWSKIN_HPP_INCLUDED

#include <vector>

#include "../common/RefCountedObject.hpp"
#include "../common/RefCountedObjectPtr.hpp"
#include "Image.hpp"
#include "RGBA.hpp"


namespace rpgss {
    namespace graphics {

        class WindowSkin : public RefCountedObject {
        public:
            typedef RefCountedObjectPtr<WindowSkin> Ptr;

        public:
            static WindowSkin::Ptr New(Image* windowSkinImage);

        public:
            enum Border {
                TopBorder = 0,
                RightBorder,
                BottomBorder,
                LeftBorder,
                TopLeftBorder,
                TopRightBorder,
                BottomRightBorder,
                BottomLeftBorder,
            };

            enum BgColor {
                TopLeftBgColor = 0,
                TopRightBgColor,
                BottomRightBgColor,
                BottomLeftBgColor,
            };

            const Image* getBorderImage(Border index) const;
            RGBA getBgColor(BgColor index) const;
            void setBgColor(BgColor index, RGBA newColor);

        private:
            // use New()
            WindowSkin();
            ~WindowSkin();

            bool initFromImage(Image* windowSkinImage);

        private:
            std::vector<Image::Ptr> _borderImages;
            std::vector<RGBA> _bgColors;
        };

        //-----------------------------------------------------------------
        inline const Image*
        WindowSkin::getBorderImage(Border index) const
        {
            return _borderImages[index];
        }

        //-----------------------------------------------------------------
        inline RGBA
        WindowSkin::getBgColor(BgColor index) const
        {
            return _bgColors[index];
        }

        //-----------------------------------------------------------------
        inline void
        WindowSkin::setBgColor(BgColor index, RGBA newColor)
        {
            _bgColors[index] = newColor;
        }

    } // namespace graphics
} // namespace rpgss


#endif // RPGSS_GRAPHICS_WINDOWSKIN_HPP_INCLUDED
