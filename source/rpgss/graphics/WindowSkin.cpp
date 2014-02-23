#include <cassert>

#include "../core/Rect.hpp"
#include "WindowSkin.hpp"

#define RPGSS_MIN_BORDER_LEN 2


namespace rpgss {
    namespace graphics {

        //-----------------------------------------------------------------
        WindowSkin::Ptr
        WindowSkin::New(Image* windowSkinImage)
        {
            assert(windowSkinImage);

            if (!windowSkinImage) {
                return 0;
            }

            WindowSkin::Ptr windowSkin = new WindowSkin();

            if (!windowSkin->initFromImage(windowSkinImage)) {
                return 0;
            }

            return windowSkin;
        }

        //-----------------------------------------------------------------
        WindowSkin::WindowSkin()
        {
        }

        //-----------------------------------------------------------------
        WindowSkin::~WindowSkin()
        {
        }

        //-----------------------------------------------------------------
        bool
        WindowSkin::initFromImage(Image* windowSkinImage)
        {
            assert(windowSkinImage);

            // enforce minimum size
            if (windowSkinImage->getWidth()  < 4 + 3 * RPGSS_MIN_BORDER_LEN ||
                windowSkinImage->getHeight() < 4 + 3 * RPGSS_MIN_BORDER_LEN)
            {
                return false;
            }

            RGBA skip_color = windowSkinImage->getPixel(0, 0);

            /*
             * find top left edge image
             */

            int tl_x1 = 1;
            int tl_y1 = 1;
            int tl_x2 = 1;
            int tl_y2 = 1;

            for (int i = tl_x2; i < windowSkinImage->getWidth() - 1; i++) {
                if (windowSkinImage->getPixel(i, tl_y1) == skip_color) {
                    tl_x2 = i - 1;
                    break;
                }
            }

            for (int i = tl_y2; i < windowSkinImage->getHeight() - 1; i++) {
                if (windowSkinImage->getPixel(tl_x1, i) == skip_color) {
                    tl_y2 = i - 1;
                    break;
                }
            }

            /*
             * find top right edge image
             */

            int tr_x1 = windowSkinImage->getWidth() - 2;
            int tr_y1 = 1;
            int tr_x2 = windowSkinImage->getWidth() - 2;
            int tr_y2 = 1;

            for (int i = tr_x1; i > 0; i--) {
                if (windowSkinImage->getPixel(i, tr_y1) == skip_color) {
                    tr_x1 = i + 1;
                    break;
                }
            }

            for (int i = tr_y2; i < windowSkinImage->getHeight() - 1; i++) {
                if (windowSkinImage->getPixel(tr_x1, i) == skip_color) {
                    tr_y2 = i - 1;
                    break;
                }
            }

            /*
             * find bottom right edge image
             */

            int br_x1 = windowSkinImage->getWidth() - 2;
            int br_y1 = windowSkinImage->getHeight() - 2;
            int br_x2 = windowSkinImage->getWidth() - 2;
            int br_y2 = windowSkinImage->getHeight() - 2;

            for (int i = br_x1; i > 0; i--) {
                if (windowSkinImage->getPixel(i, br_y1) == skip_color) {
                    br_x1 = i + 1;
                    break;
                }
            }

            for (int i = br_y1; i > 0; i--) {
                if (windowSkinImage->getPixel(br_x1, i) == skip_color) {
                    br_y1 = i + 1;
                    break;
                }
            }

            /*
             * find bottom left edge image
             */

            int bl_x1 = 1;
            int bl_y1 = windowSkinImage->getHeight() - 2;
            int bl_x2 = 1;
            int bl_y2 = windowSkinImage->getHeight() - 2;

            for (int i = bl_x2; i < windowSkinImage->getWidth() - 1; i++) {
                if (windowSkinImage->getPixel(i, bl_y2) == skip_color) {
                    bl_x2 = i - 1;
                    break;
                }
            }

            for (int i = bl_y1; i > 0; i--) {
                if (windowSkinImage->getPixel(bl_x1, i) == skip_color) {
                    bl_y1 = i + 1;
                    break;
                }
            }

            /*
             * make sure the found boundaries are valid
             */

            if ((tr_x1 - 2) - (tl_x2 + 2) + 1 < RPGSS_MIN_BORDER_LEN ||
                (br_x1 - 2) - (bl_x2 + 2) + 1 < RPGSS_MIN_BORDER_LEN ||
                (bl_y1 - 2) - (tl_y2 + 2) + 1 < RPGSS_MIN_BORDER_LEN ||
                (br_y1 - 2) - (tr_y2 + 2) + 1 < RPGSS_MIN_BORDER_LEN)
            {
                return false;
            }

            /*
             * extract images
             */

             _borderImages.resize(8);

            _borderImages[TopLeftBorder]     = windowSkinImage->copyRect(Recti(tl_x1, tl_y1, (tl_x2 - tl_x1) + 1, (tl_y2 - tl_y1) + 1));
            _borderImages[TopRightBorder]    = windowSkinImage->copyRect(Recti(tr_x1, tr_y1, (tr_x2 - tr_x1) + 1, (tr_y2 - tr_y1) + 1));

            _borderImages[BottomLeftBorder]  = windowSkinImage->copyRect(Recti(bl_x1, bl_y1, (bl_x2 - bl_x1) + 1, (bl_y2 - bl_y1) + 1));
            _borderImages[BottomRightBorder] = windowSkinImage->copyRect(Recti(br_x1, br_y1, (br_x2 - br_x1) + 1, (br_y2 - br_y1) + 1));

            _borderImages[TopBorder]         = windowSkinImage->copyRect(Recti(tl_x2 + 2, tl_y1, (tr_x1 - 2) - (tl_x2 + 2) + 1, (tl_y2 - tl_y1) + 1));
            _borderImages[BottomBorder]      = windowSkinImage->copyRect(Recti(bl_x2 + 2, bl_y1, (br_x1 - 2) - (bl_x2 + 2) + 1, (bl_y2 - bl_y1) + 1));

            _borderImages[LeftBorder]        = windowSkinImage->copyRect(Recti(tl_x1, tl_y2 + 2, (tl_x2 - tl_x1) + 1, (bl_y1 - 2) - (tl_y2 + 2) + 1));
            _borderImages[RightBorder]       = windowSkinImage->copyRect(Recti(tr_x1, tr_y2 + 2, (tr_x2 - tr_x1) + 1, (br_y1 - 2) - (tr_y2 + 2) + 1));

            /*
             * extract background colors
             */

            _bgColors.resize(4);

            _bgColors[TopLeftBgColor]     = windowSkinImage->getPixel(tl_x2 + 2, tl_y2 + 2);
            _bgColors[TopRightBgColor]    = windowSkinImage->getPixel(tr_x1 - 2, tr_y2 + 2);
            _bgColors[BottomRightBgColor] = windowSkinImage->getPixel(br_x1 - 2, br_y1 - 2);
            _bgColors[BottomLeftBgColor]  = windowSkinImage->getPixel(bl_x2 + 2, bl_y1 - 2);

            return true;
        }

    } // namespace graphics
} // namespace rpgss
