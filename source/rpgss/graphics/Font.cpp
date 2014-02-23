#include <cassert>
#include <cstring>

#include "../core/Rect.hpp"
#include "Font.hpp"

#define RPGSS_MIN_FONT_CHAR_WIDTH  2
#define RPGSS_MIN_FONT_CHAR_HEIGHT 2


namespace rpgss {
    namespace graphics {

        //-----------------------------------------------------------------
        Font::Ptr
        Font::New(Image* fontImage)
        {
            assert(fontImage);

            if (!fontImage) {
                return 0;
            }

            Font::Ptr font = new Font();

            if (!font->initFromImage(fontImage)) {
                return 0;
            }

            return font;
        }

        //-----------------------------------------------------------------
        Font::Font()
            : _maxCharWidth(0)
            , _maxCharHeight(0)
            , _tabWidth(4)
        {
        }

        //-----------------------------------------------------------------
        Font::~Font()
        {
        }

        //-----------------------------------------------------------------
        bool
        Font::initFromImage(Image* fontImage)
        {
            assert(fontImage);

            // enforce minimum char size
            if (fontImage->getWidth()  < 17 + 16 * RPGSS_MIN_FONT_CHAR_WIDTH ||
                fontImage->getHeight() < 17 + 16 * RPGSS_MIN_FONT_CHAR_HEIGHT)
            {
                return false;
            }

            // enforce char size consistency
            if ((fontImage->getWidth()  - 17) % ((fontImage->getWidth()  - 17) / 16) != 0 ||
                (fontImage->getHeight() - 17) % ((fontImage->getHeight() - 17) / 16) != 0)
            {
                return false;
            }

            _chars.resize(256);
            _maxCharWidth  = (fontImage->getWidth()  - 17) / 16;
            _maxCharHeight = (fontImage->getHeight() - 17) / 16;

            RGBA skip_color = fontImage->getPixel(0, 0);

            // extract char images
            for (int char_index = 0; char_index < 256; char_index++)
            {
                int char_row    = (char_index / 16);
                int char_column = (char_index % 16);

                int char_x      = 1 + char_column + char_column * _maxCharWidth;
                int char_y      = 1 + char_row    + char_row    * _maxCharHeight;
                int char_width  = _maxCharWidth;
                int char_height = _maxCharHeight;

                if (fontImage->getPixel(char_x, char_y) != skip_color) { // if char is not empty
                    // allow variable char width
                    for (int i = 0; i < _maxCharWidth; i++) {
                        if (fontImage->getPixel(char_x + i, char_y) == skip_color) {
                            char_width = i;
                            break;
                        }
                    }

                    // extract char image
                    Recti image_rect = Recti(char_x, char_y, char_width, char_height);
                    _chars[char_index] = fontImage->copyRect(image_rect);
                }
            }

            return true;
        }

        //-----------------------------------------------------------------
        int
        Font::getStringWidth(const char* str, int len)
        {
            assert(str);

            if (len < 0) {
                len = std::strlen(str);
            }

            int str_width = 0;

            for (int i = 0; i < len; i++) {
                if (str[i] == '\n') {
                    break;
                }
                Image* char_image = _chars[(u8)str[i]];
                str_width += (char_image ? char_image->getWidth() : 0);
            }

            return str_width;
        }

        //-----------------------------------------------------------------
        void
        Font::wordWrapString(const char* str, int len, int max_line_width, std::vector<std::pair<int, int> >& lines)
        {
            if (len < 0) {
                len = std::strlen(str);
            }

            if (len == 0 || max_line_width < _maxCharWidth) {
                return;
            }

            int lc = 0; // line char count
            int lw = 0; // line width
            int wc = 0; // word char count
            int ww = 0; // word width

            for (int i = 0; i < len; i++)
            {
                switch (str[i])
                {
                    case ' ':
                    {
                        lc += wc;
                        lw += ww;
                        wc = 0;
                        ww = 0;

                        Image* space_char_image = _chars[(u8)' '];
                        int space_w = (space_char_image ? space_char_image->getWidth() : 0);

                        if (lw + space_w > max_line_width && lw > 0) {
                            lines.push_back(std::make_pair(i-lc, lc));
                            lc = 0;
                            lw = 0;
                        } else {
                            lc++;
                            lw += space_w;
                        }
                        break;
                    }
                    case '\t':
                    {
                        lc += wc;
                        lw += ww;
                        wc = 0;
                        ww = 0;

                        Image* space_char_image = _chars[(u8)' '];
                        int tab_w = (space_char_image ? space_char_image->getWidth() : 0) * _tabWidth;

                        if (tab_w > 0) {
                            tab_w = tab_w - (lw % tab_w);
                        }

                        if (lw + tab_w > max_line_width && lw > 0) {
                            lines.push_back(std::make_pair(i-lc, lc));
                            lc = 0;
                            lw = 0;
                        } else {
                            lc++;
                            lw += tab_w;
                        }
                        break;
                    }
                    case '\n':
                    {
                        if (lc + wc > 0) {
                            lines.push_back(std::make_pair(i-(lc+wc), lc+wc));
                            lc = 0;
                            lw = 0;
                            wc = 0;
                            ww = 0;
                        } else {
                            lines.push_back(std::make_pair(i, 0));
                        }
                        break;
                    }
                    default:
                    {
                        Image* char_image = _chars[(u8)str[i]];
                        int char_w = (char_image ? char_image->getWidth() : 0);

                        if (lw + ww + char_w > max_line_width) {
                            if (lw > 0) {
                                lines.push_back(std::make_pair(i-(lc+wc), lc));
                                lc = 0;
                                lw = 0;
                            }
                            if (ww + char_w > max_line_width) {
                                lines.push_back(std::make_pair(i-wc, wc));
                                wc = 0;
                                ww = 0;
                            }
                        }
                        wc++;
                        ww += char_w;
                        break;
                    }
                }
            }

            if (lw + ww > 0) {
                lines.push_back(std::make_pair(len-(lc+wc), lc+wc));
            }
        }

    } // namespace graphics
} // namespace rpgss
