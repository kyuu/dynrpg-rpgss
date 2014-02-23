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

            int getStringWidth(const char* str, int len);
            void wordWrapString(const char* str, int len, int max_line_width, std::vector<std::pair<int, int> >& lines);

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
