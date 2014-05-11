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

#ifndef RPGSS_GRAPHICS_FONT_HPP_INCLUDED
#define RPGSS_GRAPHICS_FONT_HPP_INCLUDED

#include <string>
#include <vector>

#include "../common/RefCountedObject.hpp"
#include "../common/RefCountedObjectPtr.hpp"
#include "Image.hpp"


namespace rpgss {
    namespace graphics {

        class Font : public RefCountedObject {
        public:
            typedef RefCountedObjectPtr<Font> Ptr;

        public:
            static Font::Ptr New(Image* atlas);

        public:
            int getMaxCharWidth() const;
            int getMaxCharHeight() const;

            int getTabWidth() const;
            void setTabWidth(int tabWidth);

            const Image* getCharImage(char c) const;

            int getTextWidth(const char* str, int len);
            void wordWrapText(const char* str, int len, int max_line_width, std::vector<std::pair<int, int> >& lines);

        private:
            // use New()
            Font();
            ~Font();

            bool initFromImage(Image* fontImage);

        private:
            std::vector<Image::Ptr> _chars;
            int _maxCharWidth;
            int _maxCharHeight;
            int _tabWidth;
        };

        //-----------------------------------------------------------------
        inline int
        Font::getMaxCharWidth() const
        {
            return _maxCharWidth;
        }

        //-----------------------------------------------------------------
        inline int
        Font::getMaxCharHeight() const
        {
            return _maxCharHeight;
        }

        //-----------------------------------------------------------------
        inline int
        Font::getTabWidth() const
        {
            return _tabWidth;
        }

        //-----------------------------------------------------------------
        inline void
        Font::setTabWidth(int tabWidth)
        {
            _tabWidth = (tabWidth >= 0 ? tabWidth : 0);
        }

        inline const Image*
        Font::getCharImage(char c) const
        {
            return _chars[(u8)c];
        }

    } // namespace graphics
} // namespace rpgss


#endif // RPGSS_GRAPHICS_FONT_HPP_INCLUDED
