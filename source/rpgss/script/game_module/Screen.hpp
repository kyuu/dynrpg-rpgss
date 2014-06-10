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

#ifndef RPGSS_SCRIPT_GAME_MODULE_SCREEN_HPP_INCLUDED
#define RPGSS_SCRIPT_GAME_MODULE_SCREEN_HPP_INCLUDED

#include "../../common/types.hpp"
#include "../../core/Vec2.hpp"
#include "../../core/Rect.hpp"
#include "../../graphics/Image.hpp"


namespace rpgss {
    namespace script {
        namespace game_module {

            class Screen {
            public:
                static int GetWidth();
                static int GetHeight();
                static int GetPitch();
                static u16* GetPixels();
                static const core::Recti& GetClipRect();
                static void SetClipRect(const core::Recti& clipRect);
                static graphics::RGBA ApplyBrightness(graphics::RGBA color);

                static u16 GetPixel(int x, int y);
                static void SetPixel(int x, int y, u16 color);

                static graphics::Image::Ptr CopyRect(const core::Recti& rect, graphics::Image* destination = 0);
                static void Clear(graphics::RGBA color = graphics::RGBA(0, 0, 0));
                static void Grey();

                static void DrawPoint(const core::Vec2i& pos, graphics::RGBA color, int blendMode = graphics::BlendMode::Mix);
                static void DrawLine(const core::Vec2i& p1, const core::Vec2i& p2, graphics::RGBA c1, graphics::RGBA c2, int blendMode = graphics::BlendMode::Mix);
                static void DrawRectangle(bool fill, const core::Recti& rect, graphics::RGBA c1, graphics::RGBA c2, graphics::RGBA c3, graphics::RGBA c4, int blendMode = graphics::BlendMode::Mix);
                static void DrawCircle(bool fill, const core::Vec2i& center, int radius, graphics::RGBA c1, graphics::RGBA c2, int blendMode = graphics::BlendMode::Mix);
                static void DrawTriangle(bool fill, const core::Vec2i& p1, const core::Vec2i& p2, const core::Vec2i& p3, graphics::RGBA c1, graphics::RGBA c2, graphics::RGBA c3, int blendMode = graphics::BlendMode::Mix);
                static void Draw(const graphics::Image* image, const core::Recti& image_rect, const core::Vec2i& pos, float angle = 0.0, float scale = 1.0, graphics::RGBA color = graphics::RGBA(255, 255, 255, 255), int blendMode = graphics::BlendMode::Mix);
                static void Drawq(const graphics::Image* image, const core::Recti& image_rect, const core::Vec2i& ul, const core::Vec2i& ur, const core::Vec2i& lr, const core::Vec2i& ll, graphics::RGBA color = graphics::RGBA(255, 255, 255, 255), int blendMode = graphics::BlendMode::Mix);
                static void DrawText(const graphics::Font* font, core::Vec2i pos, const char* text, int len = -1, float scale = 1.0, graphics::RGBA color = graphics::RGBA(255, 255, 255, 255));
                static void DrawWindow(const graphics::WindowSkin* windowSkin, core::Recti windowRect);

            private:
                Screen(); // non-instantiable
                static void Clear_generic(graphics::RGBA color);
                static void Clear_sse2(graphics::RGBA color);

            private:
                static core::Recti _clipRect;
            };

        } // game_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_GAME_MODULE_SCREEN_HPP_INCLUDED
