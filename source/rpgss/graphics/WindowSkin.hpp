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

    } // namespace graphics
} // namespace rpgss


#endif // RPGSS_GRAPHICS_WINDOWSKIN_HPP_INCLUDED
