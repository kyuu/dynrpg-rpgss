#include <cstring>
#include <cstdlib>
#include <algorithm>

#include <emmintrin.h>

#include "../debug/debug.hpp"
#include "primitives.hpp"
#include "cpuinfo.hpp"
#include "Font.hpp"
#include "WindowSkin.hpp"
#include "Image.hpp"


namespace rpgss {
    namespace graphics {

        //-----------------------------------------------------------------
        Image::Ptr
        Image::New(int width, int height)
        {
            if (width * height <= 0) {
                return 0;
            }
            return new Image(width, height);
        }

        //-----------------------------------------------------------------
        Image::Ptr
        Image::New(int width, int height, RGBA color)
        {
            if (width * height <= 0) {
                return 0;
            }
            return new Image(width, height, color);
        }

        //-----------------------------------------------------------------
        Image::Ptr
        Image::New(int width, int height, const RGBA* pixels)
        {
            if (width * height <= 0 || !pixels) {
                return 0;
            }
            return new Image(width, height, pixels);
        }

        //-----------------------------------------------------------------
        Image::Image(int width, int height)
            : _width(width)
            , _height(height)
            , _pixels(0)
            , _clipRect(width, height)
            , _blendMode(BlendMode::Mix)
        {
            _pixels = allocatePixels(width, height);
        }

        //-----------------------------------------------------------------
        Image::Image(int width, int height, RGBA color)
            : _width(width)
            , _height(height)
            , _pixels(0)
            , _clipRect(width, height)
            , _blendMode(BlendMode::Mix)
        {
            _pixels = allocatePixels(width, height);
            clear(color);
        }

        //-----------------------------------------------------------------
        Image::Image(int width, int height, const RGBA* pixels)
            : _width(width)
            , _height(height)
            , _pixels(0)
            , _clipRect(width, height)
            , _blendMode(BlendMode::Mix)
        {
            _pixels = allocatePixels(width, height);
            std::memcpy(_pixels, pixels, getSizeInBytes());
        }

        //-----------------------------------------------------------------
        Image::~Image()
        {
            deletePixels(_pixels);
        }

        //-----------------------------------------------------------------
        RGBA*
        Image::allocatePixels(int width, int height)
        {
            return (RGBA*)std::malloc(width * height * sizeof(RGBA));
        }

        //-----------------------------------------------------------------
        void
        Image::deletePixels(RGBA* pixels)
        {
            std::free(pixels);
        }

        //-----------------------------------------------------------------
        void
        Image::reset(int new_width, int new_height, RGBA* new_pixels)
        {
            deletePixels(_pixels);
            _width    = new_width;
            _height   = new_height;
            _pixels   = new_pixels;
            _clipRect = Recti(new_width, new_height);
        }

        //-----------------------------------------------------------------
        void
        Image::setClipRect(const Recti& clipRect)
        {
            if (clipRect.isValid() && clipRect.isInside(0, 0, _width, _height)) {
                _clipRect = clipRect;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::setBlendMode(int blendMode)
        {
            switch (blendMode) {
            case BlendMode::Set:
            case BlendMode::Mix:
            case BlendMode::Add:
            case BlendMode::Subtract:
            case BlendMode::Multiply:
                _blendMode = blendMode;
                break;
            default:
                break;
            }
        }

        //-----------------------------------------------------------------
        Image::Ptr
        Image::copyRect_generic(const Recti& rect, Image* destination)
        {
            if (rect.isEmpty() || !rect.isInside(0, 0, _width, _height)) {
                return 0;
            }

            int rw = rect.getWidth();
            int rh = rect.getHeight();

            Image::Ptr image;

            if (destination) {
                image = destination;
                image->resize(rw, rh);
            } else {
                image = New(rw, rh);
            }

            for (int iy = 0; iy < rh; ++iy) {
                std::memcpy(
                    image->getPixels() + iy * image->getWidth(),
                    _pixels + (iy + rect.getY()) * _width + rect.getX(),
                    image->getWidth() * sizeof(RGBA)
                );
            }

            return image;
        }

        //-----------------------------------------------------------------
        Image::Ptr
        Image::copyRect_sse2(const Recti& rect, Image* destination)
        {
            if (rect.isEmpty() || !rect.isInside(0, 0, _width, _height)) {
                return 0;
            }

            Image::Ptr image;

            if (destination) {
                image = destination;
                image->resize(rect.getWidth(), rect.getHeight());
            } else {
                image = New(rect.getWidth(), rect.getHeight());
            }

            int   si = getWidth() - rect.getWidth();
            RGBA* sp = getPixels() + rect.getY() * getWidth() + rect.getX();

            RGBA* dp = image->getPixels();

            int burst1 = rect.getWidth() / 4;
            int burst2 = rect.getWidth() % 4;

            for (int y = 0; y < rect.getHeight(); y++) {
                for (int i = 0; i < burst1; i++) {
                    __m128i ms = _mm_loadu_si128((__m128i*)sp);
                    _mm_storeu_si128((__m128i*)dp, ms);
                    dp += 4;
                    sp += 4;
                }

                for (int i = 0; i < burst2; i++) {
                    *dp = *sp;
                    dp++;
                    sp++;
                }

                sp += si;
            }

            return image;
        }

        //-------------------- ---------------------------------------------
        Image::Ptr
        Image::copyRect(const Recti& rect, Image* destination)
        {
            if (CPUSupportsSSE2()) {
                return copyRect_sse2(rect, destination);
            } else {
                return copyRect_generic(rect, destination);
            }
        }

        //-----------------------------------------------------------------
        void
        Image::resize(int new_width, int new_height)
        {
            if ((new_width <= 0 || new_height <= 0) || (new_width == _width && new_height == _height)) {
                return;
            }

            RGBA* new_pixels = allocatePixels(new_width, new_height);

            for (int i = 0; i < std::min(_height, new_height); ++i) {
                std::memcpy(
                    new_pixels + (i * new_width),
                    _pixels + (i * _width),
                    std::min(_width, new_width) * sizeof(RGBA)
                );
            }
            reset(new_width, new_height, new_pixels);
        }

        //-----------------------------------------------------------------
        void
        Image::setAlpha(u8 alpha)
        {
            RGBA* p = _pixels;
            int   i = _width * _height;
            while (i > 0) {
                p->alpha = alpha;
                ++p;
                --i;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::clear_generic(RGBA color)
        {
            u32* p = (u32*)_pixels;
            u32  q = *((u32*)&color);
            int  i = _width * _height;
            while (i > 0) {
                *p = q;
                ++p;
                --i;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::clear_sse2(RGBA color)
        {
            int num_pixels = _width * _height;

            int burst1 = ((int)_pixels % 16) / 4;
            int burst2 = (num_pixels - burst1) / 4;
            int burst3 = num_pixels - (burst1 + burst2 * 4);

            RGBA* dp = _pixels;

            for (int i = 0; i < burst1; i++) {
                *dp = color;
                dp++;
            }

            __m128i mcolor  = _mm_set_epi8(
                color.alpha, color.blue, color.green, color.red,
                color.alpha, color.blue, color.green, color.red,
                color.alpha, color.blue, color.green, color.red,
                color.alpha, color.blue, color.green, color.red
            );

            for (int i = 0; i < burst2; i++) {
                _mm_store_si128((__m128i*)dp, mcolor);
                dp += 4;
            }

            for (int i = 0; i < burst3; i++) {
                *dp = color;
                dp++;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::clear(RGBA color)
        {
            if (CPUSupportsSSE2()) {
                return clear_sse2(color);
            } else {
                return clear_generic(color);
            }
        }

        //-----------------------------------------------------------------
        void
        Image::grey()
        {
            RGBA* p = _pixels;
            int   i = _width * _height;
            while (i > 0) {
                u8 q = (p->red + p->green + p->blue) / 3;
                p->red   = q;
                p->green = q;
                p->blue  = q;
                ++p;
                --i;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::flipHorizontal()
        {
            u32* l = (u32*)_pixels;
            u32* r = (u32*)_pixels + _width - 1;

            int iy = _height;
            while (iy > 0) {
                u32* a = l;
                u32* b = r;
                int ix = _width / 2;
                while (ix > 0) {
                    u32 c = *a;
                    *a = *b;
                    *b = c;
                    a++;
                    b--;
                    ix--;
                }
                l += _width;
                r += _width;
                iy--;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::flipVertical()
        {
            u32* u = (u32*)_pixels;
            u32* d = (u32*)_pixels + _width * (_height - 1);

            int iy = _height / 2;
            while (iy > 0) {
                u32* a = u;
                u32* b = d;
                int ix = _width;
                while (ix > 0) {
                    u32 c = *a;
                    *a = *b;
                    *b = c;
                    a++;
                    b++;
                    ix--;
                }
                u += _width;
                d -= _width;
                iy--;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::rotateClockwise()
        {
            RGBA* new_p = allocatePixels(_width, _height);
            int   new_w = _height;
            int   new_h = _width;

            for (int iy = 0; iy < _height; ++iy) {
                for (int ix = 0; ix < _width; ++ix) {
                    new_p[new_w * ix + (new_w - (iy + 1))] = _pixels[iy * _width + ix];
                }
            }
            reset(new_w, new_h, new_p);
        }

        //-----------------------------------------------------------------
        void
        Image::rotateCounterClockwise()
        {
            RGBA* new_p = allocatePixels(_width, _height);
            int   new_w = _height;
            int   new_h = _width;

            for (int iy = 0; iy < _height; ++iy) {
                for (int ix = 0; ix < _width; ++ix) {
                    new_p[new_w * (_width - ix - 1) + iy] = _pixels[iy * _width + ix];
                }
            }
            reset(new_w, new_h, new_p);
        }

        //-----------------------------------------------------------------
        struct rgba_set {
            void operator()(RGBA& dst, const RGBA& src) {
                dst.red   = src.red;
                dst.green = src.green;
                dst.blue  = src.blue;
                dst.alpha = src.alpha;
            }

            void operator()(RGBA& dst, u8 red, u8 green, u8 blue, u8 alpha) {
                dst.red   = red;
                dst.green = green;
                dst.blue  = blue;
                dst.alpha = alpha;
            }
        };

        struct rgba_mix {
            void operator()(RGBA& dst, const RGBA& src) {
                int sa = src.alpha  + 1;
                int da = 256 - src.alpha;
                dst.red   = (dst.red   * da + src.red   * sa) >> 8;
                dst.green = (dst.green * da + src.green * sa) >> 8;
                dst.blue  = (dst.blue  * da + src.blue  * sa) >> 8;
            }

            void operator()(RGBA& dst, u8 red, u8 green, u8 blue, u8 alpha) {
                int sa = alpha  + 1;
                int da = 256 - alpha;
                dst.red   = (dst.red   * da + red   * sa) >> 8;
                dst.green = (dst.green * da + green * sa) >> 8;
                dst.blue  = (dst.blue  * da + blue  * sa) >> 8;
            }
        };

        struct rgba_add {
            void operator()(RGBA& dst, const RGBA& src) {
                dst.red   = std::min(dst.red   + src.red,   255);
                dst.green = std::min(dst.green + src.green, 255);
                dst.blue  = std::min(dst.blue  + src.blue,  255);
                dst.alpha = std::min(dst.alpha + src.alpha, 255);
            }

            void operator()(RGBA& dst, u8 red, u8 green, u8 blue, u8 alpha) {
                dst.red   = std::min(dst.red   + red,   255);
                dst.green = std::min(dst.green + green, 255);
                dst.blue  = std::min(dst.blue  + blue,  255);
                dst.alpha = std::min(dst.alpha + alpha, 255);
            }
        };

        struct rgba_sub {
            void operator()(RGBA& dst, const RGBA& src) {
                dst.red   = std::max(dst.red   - src.red,   0);
                dst.green = std::max(dst.green - src.green, 0);
                dst.blue  = std::max(dst.blue  - src.blue,  0);
                dst.alpha = std::max(dst.alpha - src.alpha, 0);
            }

            void operator()(RGBA& dst, u8 red, u8 green, u8 blue, u8 alpha) {
                dst.red   = std::max(dst.red   - red,   0);
                dst.green = std::max(dst.green - green, 0);
                dst.blue  = std::max(dst.blue  - blue,  0);
                dst.alpha = std::max(dst.alpha - alpha, 0);
            }
        };

        struct rgba_mul {
            void operator()(RGBA& dst, const RGBA& src) {
                dst.red   = dst.red   * (src.red   + 1) >> 8;
                dst.green = dst.green * (src.green + 1) >> 8;
                dst.blue  = dst.blue  * (src.blue  + 1) >> 8;
                dst.alpha = dst.alpha * (src.alpha + 1) >> 8;
            }

            void operator()(RGBA& dst, u8 red, u8 green, u8 blue, u8 alpha) {
                dst.red   = dst.red   * (red   + 1) >> 8;
                dst.green = dst.green * (green + 1) >> 8;
                dst.blue  = dst.blue  * (blue  + 1) >> 8;
                dst.alpha = dst.alpha * (alpha + 1) >> 8;
            }
        };

        //-----------------------------------------------------------------
        void
        Image::drawPoint(const Vec2i& pos, RGBA color)
        {
            switch (_blendMode) {
            case BlendMode::Set:      primitives::Point(_pixels, _width, _clipRect, pos, color, rgba_set()); break;
            case BlendMode::Mix:      primitives::Point(_pixels, _width, _clipRect, pos, color, rgba_mix()); break;
            case BlendMode::Add:      primitives::Point(_pixels, _width, _clipRect, pos, color, rgba_add()); break;
            case BlendMode::Subtract: primitives::Point(_pixels, _width, _clipRect, pos, color, rgba_sub()); break;
            case BlendMode::Multiply: primitives::Point(_pixels, _width, _clipRect, pos, color, rgba_mul()); break;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::drawLine(const Vec2i& startPos, const Vec2i& endPos, RGBA color)
        {
            switch (_blendMode) {
            case BlendMode::Set:      primitives::Line(_pixels, _width, _clipRect, startPos, endPos, color, rgba_set()); break;
            case BlendMode::Mix:      primitives::Line(_pixels, _width, _clipRect, startPos, endPos, color, rgba_mix()); break;
            case BlendMode::Add:      primitives::Line(_pixels, _width, _clipRect, startPos, endPos, color, rgba_add()); break;
            case BlendMode::Subtract: primitives::Line(_pixels, _width, _clipRect, startPos, endPos, color, rgba_sub()); break;
            case BlendMode::Multiply: primitives::Line(_pixels, _width, _clipRect, startPos, endPos, color, rgba_mul()); break;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::drawLine(const Vec2i& startPos, const Vec2i& endPos, RGBA startColor, RGBA endColor)
        {
            switch (_blendMode) {
            case BlendMode::Set:      primitives::Line(_pixels, _width, _clipRect, startPos, endPos, startColor, endColor, rgba_set()); break;
            case BlendMode::Mix:      primitives::Line(_pixels, _width, _clipRect, startPos, endPos, startColor, endColor, rgba_mix()); break;
            case BlendMode::Add:      primitives::Line(_pixels, _width, _clipRect, startPos, endPos, startColor, endColor, rgba_add()); break;
            case BlendMode::Subtract: primitives::Line(_pixels, _width, _clipRect, startPos, endPos, startColor, endColor, rgba_sub()); break;
            case BlendMode::Multiply: primitives::Line(_pixels, _width, _clipRect, startPos, endPos, startColor, endColor, rgba_mul()); break;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::drawRectangle(bool fill, const Recti& rect, RGBA color)
        {
            switch (_blendMode) {
            case BlendMode::Set:      primitives::Rectangle(_pixels, _width, _clipRect, fill, rect, color, rgba_set()); break;
            case BlendMode::Mix:      primitives::Rectangle(_pixels, _width, _clipRect, fill, rect, color, rgba_mix()); break;
            case BlendMode::Add:      primitives::Rectangle(_pixels, _width, _clipRect, fill, rect, color, rgba_add()); break;
            case BlendMode::Subtract: primitives::Rectangle(_pixels, _width, _clipRect, fill, rect, color, rgba_sub()); break;
            case BlendMode::Multiply: primitives::Rectangle(_pixels, _width, _clipRect, fill, rect, color, rgba_mul()); break;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::drawRectangle(bool fill, const Recti& rect, RGBA ulColor, RGBA urColor, RGBA lrColor, RGBA llColor)
        {
            switch (_blendMode) {
            case BlendMode::Set:      primitives::Rectangle(_pixels, _width, _clipRect, fill, rect, ulColor, urColor, lrColor, llColor, rgba_set()); break;
            case BlendMode::Mix:      primitives::Rectangle(_pixels, _width, _clipRect, fill, rect, ulColor, urColor, lrColor, llColor, rgba_mix()); break;
            case BlendMode::Add:      primitives::Rectangle(_pixels, _width, _clipRect, fill, rect, ulColor, urColor, lrColor, llColor, rgba_add()); break;
            case BlendMode::Subtract: primitives::Rectangle(_pixels, _width, _clipRect, fill, rect, ulColor, urColor, lrColor, llColor, rgba_sub()); break;
            case BlendMode::Multiply: primitives::Rectangle(_pixels, _width, _clipRect, fill, rect, ulColor, urColor, lrColor, llColor, rgba_mul()); break;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::drawCircle(bool fill, const Vec2i& center, int radius, RGBA color)
        {
            switch (_blendMode) {
            case BlendMode::Set:      primitives::Circle(_pixels, _width, _clipRect, fill, center, radius, color, rgba_set()); break;
            case BlendMode::Mix:      primitives::Circle(_pixels, _width, _clipRect, fill, center, radius, color, rgba_mix()); break;
            case BlendMode::Add:      primitives::Circle(_pixels, _width, _clipRect, fill, center, radius, color, rgba_add()); break;
            case BlendMode::Subtract: primitives::Circle(_pixels, _width, _clipRect, fill, center, radius, color, rgba_sub()); break;
            case BlendMode::Multiply: primitives::Circle(_pixels, _width, _clipRect, fill, center, radius, color, rgba_mul()); break;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::drawCircle(bool fill, const Vec2i& center, int radius, RGBA innerColor, RGBA outerColor)
        {
            switch (_blendMode) {
            case BlendMode::Set:      primitives::Circle(_pixels, _width, _clipRect, fill, center, radius, innerColor, outerColor, rgba_set()); break;
            case BlendMode::Mix:      primitives::Circle(_pixels, _width, _clipRect, fill, center, radius, innerColor, outerColor, rgba_mix()); break;
            case BlendMode::Add:      primitives::Circle(_pixels, _width, _clipRect, fill, center, radius, innerColor, outerColor, rgba_add()); break;
            case BlendMode::Subtract: primitives::Circle(_pixels, _width, _clipRect, fill, center, radius, innerColor, outerColor, rgba_sub()); break;
            case BlendMode::Multiply: primitives::Circle(_pixels, _width, _clipRect, fill, center, radius, innerColor, outerColor, rgba_mul()); break;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::drawTriangle(bool fill, const Vec2i& p1, const Vec2i& p2, const Vec2i& p3, RGBA color)
        {
            // TODO
        }

        //-----------------------------------------------------------------
        void
        Image::drawTriangle(bool fill, const Vec2i& p1, const Vec2i& p2, const Vec2i& p3, RGBA c1, RGBA c2, RGBA c3)
        {
            // TODO
        }

        //-----------------------------------------------------------------
        struct rgba_set_col
        {
            RGBA c;

            explicit rgba_set_col(RGBA color)
                : c(color)
            {
            }

            void operator()(RGBA& dst, const RGBA& src) {
                dst.red   = src.red   * (c.red   + 1) >> 8;
                dst.green = src.green * (c.green + 1) >> 8;
                dst.blue  = src.blue  * (c.blue  + 1) >> 8;
                dst.alpha = src.alpha * (c.alpha + 1) >> 8;
            }
        };

        //-----------------------------------------------------------------
        struct rgba_mix_col
        {
            RGBA c;

            explicit rgba_mix_col(RGBA color)
                : c(color)
            {
            }

            void operator()(RGBA& dst, const RGBA& src) {
                int sa = (src.alpha * (c.alpha + 1) >> 8) + 1;
                int da = 256 - (sa - 1);
                dst.red   = (dst.red   * da + (src.red   * (c.red   + 1) >> 8) * sa) >> 8;
                dst.green = (dst.green * da + (src.green * (c.green + 1) >> 8) * sa) >> 8;
                dst.blue  = (dst.blue  * da + (src.blue  * (c.blue  + 1) >> 8) * sa) >> 8;
            }
        };

        //-----------------------------------------------------------------
        struct rgba_add_col
        {
            RGBA c;

            explicit rgba_add_col(RGBA color)
                : c(color)
            {
            }

            void operator()(RGBA& dst, const RGBA& src) {
                dst.red   = std::min(dst.red   + (src.red   * (c.red   + 1) >> 8), 255);
                dst.green = std::min(dst.green + (src.green * (c.green + 1) >> 8), 255);
                dst.blue  = std::min(dst.blue  + (src.blue  * (c.blue  + 1) >> 8), 255);
                dst.alpha = std::min(dst.alpha + (src.alpha * (c.alpha + 1) >> 8), 255);
            }
        };

        //-----------------------------------------------------------------
        struct rgba_sub_col
        {
            RGBA c;

            explicit rgba_sub_col(RGBA color)
                : c(color)
            {
            }

            void operator()(RGBA& dst, const RGBA& src) {
                dst.red   = std::max(dst.red   - (src.red   * (c.red   + 1) >> 8), 0);
                dst.green = std::max(dst.green - (src.green * (c.green + 1) >> 8), 0);
                dst.blue  = std::max(dst.blue  - (src.blue  * (c.blue  + 1) >> 8), 0);
                dst.alpha = std::max(dst.alpha - (src.alpha * (c.alpha + 1) >> 8), 0);
            }
        };

        //-----------------------------------------------------------------
        struct rgba_mul_col
        {
            RGBA c;

            explicit rgba_mul_col(RGBA color)
                : c(color)
            {
            }

            void operator()(RGBA& dst, const RGBA& src) {
                dst.red   = dst.red   * ((src.red   * (c.red   + 1) >> 8) + 1) >> 8;
                dst.green = dst.green * ((src.green * (c.green + 1) >> 8) + 1) >> 8;
                dst.blue  = dst.blue  * ((src.blue  * (c.blue  + 1) >> 8) + 1) >> 8;
                dst.alpha = dst.alpha * ((src.alpha * (c.alpha + 1) >> 8) + 1) >> 8;
            }
        };

        //-----------------------------------------------------------------
        void
        Image::draw_sse2_set(const Image* image, const Recti& image_rect, const Vec2i& pos)
        {
            if (_clipRect.isEmpty() || image_rect.isEmpty()) {
                return;
            }

            Recti dst_rect = Recti(pos, image_rect.getDimensions()).getIntersection(_clipRect);

            if (dst_rect.isEmpty()) {
                return;
            }

            Recti src_rect = Recti(image_rect.getPosition() + (dst_rect.getPosition() - pos), dst_rect.getDimensions());

            rgba_set fallback_renderer;

            int di = _width - dst_rect.getWidth();
            RGBA* dp = _pixels + dst_rect.getY() * _width + dst_rect.getX();

            int si = image->getWidth() - src_rect.getWidth();
            const RGBA* sp = image->getPixels() + src_rect.getY() * image->getWidth() + src_rect.getX();

            int num_blocks    = dst_rect.getWidth() / 4;
            int num_remaining = dst_rect.getWidth() % 4;

            int iy = dst_rect.getHeight();
            while (iy > 0) {

                int ix = num_blocks;
                while (ix > 0) {
                    __m128i ms;

                    // load
                    ms = _mm_loadu_si128((__m128i*)sp);

                    // store
                    _mm_storeu_si128((__m128i*)dp, ms);

                    dp += 4;
                    sp += 4;
                    ix--;
                }

                ix = num_remaining;
                while (ix > 0) {
                    fallback_renderer(*dp, *sp);
                    dp++;
                    sp++;
                    ix--;
                }

                dp += di;
                sp += si;
                iy--;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::draw_sse2_add(const Image* image, const Recti& image_rect, const Vec2i& pos)
        {
            if (_clipRect.isEmpty() || image_rect.isEmpty()) {
                return;
            }

            Recti dst_rect = Recti(pos, image_rect.getDimensions()).getIntersection(_clipRect);

            if (dst_rect.isEmpty()) {
                return;
            }

            Recti src_rect = Recti(image_rect.getPosition() + (dst_rect.getPosition() - pos), dst_rect.getDimensions());

            rgba_add fallback_renderer;

            int di = _width - dst_rect.getWidth();
            RGBA* dp = _pixels + dst_rect.getY() * _width + dst_rect.getX();

            int si = image->getWidth() - src_rect.getWidth();
            const RGBA* sp = image->getPixels() + src_rect.getY() * image->getWidth() + src_rect.getX();

            int num_blocks    = dst_rect.getWidth() / 4;
            int num_remaining = dst_rect.getWidth() % 4;

            int iy = dst_rect.getHeight();
            while (iy > 0) {

                int ix = num_blocks;
                while (ix > 0) {

                    // load
                    __m128i md = _mm_loadu_si128((__m128i*)dp);
                    __m128i ms = _mm_loadu_si128((__m128i*)sp);

                    // add
                    md = _mm_adds_epu8(md, ms);

                    // store
                    _mm_storeu_si128((__m128i*)dp, md);

                    dp += 4;
                    sp += 4;
                    ix--;
                }

                ix = num_remaining;
                while (ix > 0) {
                    fallback_renderer(*dp, *sp);
                    dp++;
                    sp++;
                    ix--;
                }

                dp += di;
                sp += si;
                iy--;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::draw_sse2_sub(const Image* image, const Recti& image_rect, const Vec2i& pos)
        {
            if (_clipRect.isEmpty() || image_rect.isEmpty()) {
                return;
            }

            Recti dst_rect = Recti(pos, image_rect.getDimensions()).getIntersection(_clipRect);

            if (dst_rect.isEmpty()) {
                return;
            }

            Recti src_rect = Recti(image_rect.getPosition() + (dst_rect.getPosition() - pos), dst_rect.getDimensions());

            rgba_sub fallback_renderer;

            int di = _width - dst_rect.getWidth();
            RGBA* dp = _pixels + dst_rect.getY() * _width + dst_rect.getX();

            int si = image->getWidth() - src_rect.getWidth();
            const RGBA* sp = image->getPixels() + src_rect.getY() * image->getWidth() + src_rect.getX();

            int num_blocks    = dst_rect.getWidth() / 4;
            int num_remaining = dst_rect.getWidth() % 4;

            int iy = dst_rect.getHeight();
            while (iy > 0) {

                int ix = num_blocks;
                while (ix > 0) {

                    // load
                    __m128i md = _mm_loadu_si128((__m128i*)dp);
                    __m128i ms = _mm_loadu_si128((__m128i*)sp);

                    // add
                    md = _mm_subs_epu8(md, ms);

                    // store
                    _mm_storeu_si128((__m128i*)dp, md);

                    dp += 4;
                    sp += 4;
                    ix--;
                }

                ix = num_remaining;
                while (ix > 0) {
                    fallback_renderer(*dp, *sp);
                    dp++;
                    sp++;
                    ix--;
                }

                dp += di;
                sp += si;
                iy--;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::draw_sse2_mul(const Image* image, const Recti& image_rect, const Vec2i& pos)
        {
            if (_clipRect.isEmpty() || image_rect.isEmpty()) {
                return;
            }

            Recti dst_rect = Recti(pos, image_rect.getDimensions()).getIntersection(_clipRect);

            if (dst_rect.isEmpty()) {
                return;
            }

            Recti src_rect = Recti(image_rect.getPosition() + (dst_rect.getPosition() - pos), dst_rect.getDimensions());

            rgba_mul fallback_renderer;

            int di = _width - dst_rect.getWidth();
            RGBA* dp = _pixels + dst_rect.getY() * _width + dst_rect.getX();

            int si = image->getWidth() - src_rect.getWidth();
            const RGBA* sp = image->getPixels() + src_rect.getY() * image->getWidth() + src_rect.getX();

            int num_blocks    = dst_rect.getWidth() / 4;
            int num_remaining = dst_rect.getWidth() % 4;

            __m128i mnull = _mm_setzero_si128();
            __m128i mone  = _mm_set1_epi16(1);

            int iy = dst_rect.getHeight();
            while (iy > 0) {

                int ix = num_blocks;
                while (ix > 0) {

                    // load
                    __m128i md = _mm_loadu_si128((__m128i*)dp);
                    __m128i ms = _mm_loadu_si128((__m128i*)sp);

                    // multiply first two
                    __m128i t1 = _mm_unpacklo_epi8(ms, mnull);
                    __m128i t2 = _mm_unpacklo_epi8(md, mnull);
                    t1 = _mm_adds_epu16(t1, mone);
                    t1 = _mm_mullo_epi16(t1, t2);
                    t1 = _mm_srli_epi16(t1, 8);

                    // multiply next two
                    __m128i u1 = _mm_unpackhi_epi8(ms, mnull);
                    __m128i u2 = _mm_unpackhi_epi8(md, mnull);
                    u1 = _mm_adds_epu16(u1, mone);
                    u1 = _mm_mullo_epi16(u1, u2);
                    u1 = _mm_srli_epi16(u1, 8);

                    // combine
                    ms = _mm_packus_epi16(t1, u1);

                    // store
                    _mm_storeu_si128((__m128i*)dp, ms);

                    dp += 4;
                    sp += 4;
                    ix--;
                }

                ix = num_remaining;
                while (ix > 0) {
                    fallback_renderer(*dp, *sp);
                    dp++;
                    sp++;
                    ix--;
                }

                dp += di;
                sp += si;
                iy--;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::draw_sse2_set(const Image* image, const Recti& image_rect, const Vec2i& pos, RGBA color)
        {
            if (_clipRect.isEmpty() || image_rect.isEmpty()) {
                return;
            }

            Recti dst_rect = Recti(pos, image_rect.getDimensions()).getIntersection(_clipRect);

            if (dst_rect.isEmpty()) {
                return;
            }

            Recti src_rect = Recti(image_rect.getPosition() + (dst_rect.getPosition() - pos), dst_rect.getDimensions());

            rgba_set_col fallback_renderer(color);

            int di = _width - dst_rect.getWidth();
            RGBA* dp = _pixels + dst_rect.getY() * _width + dst_rect.getX();

            int si = image->getWidth() - src_rect.getWidth();
            const RGBA* sp = image->getPixels() + src_rect.getY() * image->getWidth() + src_rect.getX();

            int num_blocks    = dst_rect.getWidth() / 4;
            int num_remaining = dst_rect.getWidth() % 4;

            __m128i mcol  = _mm_set_epi16(color.alpha + 1, color.blue + 1, color.green + 1, color.red + 1, color.alpha + 1, color.blue + 1, color.green + 1, color.red + 1);
            __m128i mnull = _mm_setzero_si128();

            int iy = dst_rect.getHeight();
            while (iy > 0) {

                int ix = num_blocks;
                while (ix > 0) {
                    __m128i ms, t1, t2;

                    // load
                    ms = _mm_loadu_si128((__m128i*)sp);

                    // process
                    t1 = _mm_unpacklo_epi8(ms, mnull);
                    t2 = _mm_unpackhi_epi8(ms, mnull);
                    t1 = _mm_mullo_epi16(t1, mcol);
                    t2 = _mm_mullo_epi16(t2, mcol);
                    t1 = _mm_srli_epi16(t1, 8);
                    t2 = _mm_srli_epi16(t2, 8);
                    t1 = _mm_packus_epi16(t1, t2);

                    // store
                    _mm_storeu_si128((__m128i*)dp, t1);

                    dp += 4;
                    sp += 4;
                    ix--;
                }

                ix = num_remaining;
                while (ix > 0) {
                    fallback_renderer(*dp, *sp);
                    dp++;
                    sp++;
                    ix--;
                }

                dp += di;
                sp += si;
                iy--;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::draw_sse2_add(const Image* image, const Recti& image_rect, const Vec2i& pos, RGBA color)
        {
            if (_clipRect.isEmpty() || image_rect.isEmpty()) {
                return;
            }

            Recti dst_rect = Recti(pos, image_rect.getDimensions()).getIntersection(_clipRect);

            if (dst_rect.isEmpty()) {
                return;
            }

            Recti src_rect = Recti(image_rect.getPosition() + (dst_rect.getPosition() - pos), dst_rect.getDimensions());

            rgba_add_col fallback_renderer(color);

            int di = _width - dst_rect.getWidth();
            RGBA* dp = _pixels + dst_rect.getY() * _width + dst_rect.getX();

            int si = image->getWidth() - src_rect.getWidth();
            const RGBA* sp = image->getPixels() + src_rect.getY() * image->getWidth() + src_rect.getX();

            int num_blocks    = dst_rect.getWidth() / 4;
            int num_remaining = dst_rect.getWidth() % 4;

            __m128i mcol  = _mm_set_epi16(color.alpha + 1, color.blue + 1, color.green + 1, color.red + 1, color.alpha + 1, color.blue + 1, color.green + 1, color.red + 1);
            __m128i mnull = _mm_setzero_si128();

            int iy = dst_rect.getHeight();
            while (iy > 0) {

                int ix = num_blocks;
                while (ix > 0) {
                    __m128i md, ms, t1, t2;

                    // load
                    md = _mm_loadu_si128((__m128i*)dp);
                    ms = _mm_loadu_si128((__m128i*)sp);

                    // multiply
                    t1 = _mm_unpacklo_epi8(ms, mnull);
                    t2 = _mm_unpackhi_epi8(ms, mnull);
                    t1 = _mm_mullo_epi16(t1, mcol);
                    t2 = _mm_mullo_epi16(t2, mcol);
                    t1 = _mm_srli_epi16(t1, 8);
                    t2 = _mm_srli_epi16(t2, 8);
                    t1 = _mm_packus_epi16(t1, t2);

                    // add
                    t1 = _mm_adds_epu8(md, t1);

                    // store
                    _mm_storeu_si128((__m128i*)dp, t1);

                    dp += 4;
                    sp += 4;
                    ix--;
                }

                ix = num_remaining;
                while (ix > 0) {
                    fallback_renderer(*dp, *sp);
                    dp++;
                    sp++;
                    ix--;
                }

                dp += di;
                sp += si;
                iy--;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::draw_sse2_sub(const Image* image, const Recti& image_rect, const Vec2i& pos, RGBA color)
        {
            if (_clipRect.isEmpty() || image_rect.isEmpty()) {
                return;
            }

            Recti dst_rect = Recti(pos, image_rect.getDimensions()).getIntersection(_clipRect);

            if (dst_rect.isEmpty()) {
                return;
            }

            Recti src_rect = Recti(image_rect.getPosition() + (dst_rect.getPosition() - pos), dst_rect.getDimensions());

            rgba_sub_col fallback_renderer(color);

            int di = _width - dst_rect.getWidth();
            RGBA* dp = _pixels + dst_rect.getY() * _width + dst_rect.getX();

            int si = image->getWidth() - src_rect.getWidth();
            const RGBA* sp = image->getPixels() + src_rect.getY() * image->getWidth() + src_rect.getX();

            int num_blocks    = dst_rect.getWidth() / 4;
            int num_remaining = dst_rect.getWidth() % 4;

            __m128i mcol  = _mm_set_epi16(color.alpha + 1, color.blue + 1, color.green + 1, color.red + 1, color.alpha + 1, color.blue + 1, color.green + 1, color.red + 1);
            __m128i mnull = _mm_setzero_si128();

            int iy = dst_rect.getHeight();
            while (iy > 0) {

                int ix = num_blocks;
                while (ix > 0) {
                    __m128i md, ms, t1, t2;

                    // load
                    md = _mm_loadu_si128((__m128i*)dp);
                    ms = _mm_loadu_si128((__m128i*)sp);

                    // multiply
                    t1 = _mm_unpacklo_epi8(ms, mnull);
                    t2 = _mm_unpackhi_epi8(ms, mnull);
                    t1 = _mm_mullo_epi16(t1, mcol);
                    t2 = _mm_mullo_epi16(t2, mcol);
                    t1 = _mm_srli_epi16(t1, 8);
                    t2 = _mm_srli_epi16(t2, 8);
                    t1 = _mm_packus_epi16(t1, t2);

                    // add
                    t1 = _mm_subs_epu8(md, t1);

                    // store
                    _mm_storeu_si128((__m128i*)dp, t1);

                    dp += 4;
                    sp += 4;
                    ix--;
                }

                ix = num_remaining;
                while (ix > 0) {
                    fallback_renderer(*dp, *sp);
                    dp++;
                    sp++;
                    ix--;
                }

                dp += di;
                sp += si;
                iy--;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::draw_sse2_mul(const Image* image, const Recti& image_rect, const Vec2i& pos, RGBA color)
        {
            if (_clipRect.isEmpty() || image_rect.isEmpty()) {
                return;
            }

            Recti dst_rect = Recti(pos, image_rect.getDimensions()).getIntersection(_clipRect);

            if (dst_rect.isEmpty()) {
                return;
            }

            Recti src_rect = Recti(image_rect.getPosition() + (dst_rect.getPosition() - pos), dst_rect.getDimensions());

            rgba_mul_col fallback_renderer(color);

            int di = _width - dst_rect.getWidth();
            RGBA* dp = _pixels + dst_rect.getY() * _width + dst_rect.getX();

            int si = image->getWidth() - src_rect.getWidth();
            const RGBA* sp = image->getPixels() + src_rect.getY() * image->getWidth() + src_rect.getX();

            int num_blocks    = dst_rect.getWidth() / 4;
            int num_remaining = dst_rect.getWidth() % 4;

            __m128i mcol  = _mm_set_epi16(color.alpha + 1, color.blue + 1, color.green + 1, color.red + 1, color.alpha + 1, color.blue + 1, color.green + 1, color.red + 1);
            __m128i mnull = _mm_setzero_si128();
            __m128i mone  = _mm_set1_epi16(1);

            int iy = dst_rect.getHeight();
            while (iy > 0) {

                int ix = num_blocks;
                while (ix > 0) {
                    __m128i md, ms, t1, t2, t3;

                    // load
                    md = _mm_loadu_si128((__m128i*)dp);
                    ms = _mm_loadu_si128((__m128i*)sp);

                    // multiply first two
                    t1 = _mm_unpacklo_epi8(ms, mnull);
                    t2 = _mm_unpacklo_epi8(md, mnull);

                    t1 = _mm_mullo_epi16(t1, mcol);
                    t1 = _mm_srli_epi16(t1, 8);

                    t1 = _mm_adds_epu16(t1, mone);
                    t1 = _mm_mullo_epi16(t1, t2);
                    t1 = _mm_srli_epi16(t1, 8);

                    // multiply next two
                    t2 = _mm_unpackhi_epi8(ms, mnull);
                    t3 = _mm_unpackhi_epi8(md, mnull);

                    t2 = _mm_mullo_epi16(t2, mcol);
                    t2 = _mm_srli_epi16(t2, 8);

                    t2 = _mm_adds_epu16(t2, mone);
                    t2 = _mm_mullo_epi16(t2, t3);
                    t2 = _mm_srli_epi16(t2, 8);

                    // combine
                    t1 = _mm_packus_epi16(t1, t2);

                    // store
                    _mm_storeu_si128((__m128i*)dp, t1);

                    dp += 4;
                    sp += 4;
                    ix--;
                }

                ix = num_remaining;
                while (ix > 0) {
                    fallback_renderer(*dp, *sp);
                    dp++;
                    sp++;
                    ix--;
                }

                dp += di;
                sp += si;
                iy--;
            }
        }

        //-----------------------------------------------------------------
        void
        Image::draw(const Image* image, const Vec2i& pos, float rotate, float scale, RGBA color)
        {
            Recti image_rect = Recti(image->getDimensions());
            draw(image, image_rect, pos, rotate, scale, color);
        }

        //-----------------------------------------------------------------
        void
        Image::draw(const Image* image, const Recti& image_rect, const Vec2i& pos, float rotate, float scale, RGBA color)
        {
            if (CPUSupportsSSE2() && (rotate == 0.0 && scale == 1.0))
            {
                if (color == RGBA(255, 255, 255, 255))
                {
                    switch (_blendMode) {
                    case BlendMode::Set:
                        draw_sse2_set(image, image_rect, pos);
                        break;
                    case BlendMode::Mix:
                        // difficult to implement with SSE2 intrinsics
                        primitives::TexturedRectangle(_pixels, _width, _clipRect, pos, image->getPixels(), image->getWidth(), image_rect, rgba_mix());
                        break;
                    case BlendMode::Add:
                        draw_sse2_add(image, image_rect, pos);
                        break;
                    case BlendMode::Subtract:
                        draw_sse2_sub(image, image_rect, pos);
                        break;
                    case BlendMode::Multiply:
                        draw_sse2_mul(image, image_rect, pos);
                        break;
                    }
                }
                else
                {
                    switch (_blendMode) {
                    case BlendMode::Set:
                        draw_sse2_set(image, image_rect, pos, color);
                        break;
                    case BlendMode::Mix:
                        // difficult to implement with SSE2 intrinsics
                        primitives::TexturedRectangle(_pixels, _width, _clipRect, pos, image->getPixels(), image->getWidth(), image_rect, rgba_mix_col(color));
                        break;
                    case BlendMode::Add:
                        draw_sse2_add(image, image_rect, pos, color);
                        break;
                    case BlendMode::Subtract:
                        draw_sse2_sub(image, image_rect, pos, color);
                        break;
                    case BlendMode::Multiply:
                        draw_sse2_mul(image, image_rect, pos, color);
                        break;
                    }
                }
            }
            else if (rotate == 0.0)
            {
                Recti rect = Recti(pos, image->getDimensions()).scale(scale);

                if (color == RGBA(255, 255, 255, 255))
                {
                    switch (_blendMode) {
                    case BlendMode::Set:      primitives::TexturedRectangle(_pixels, _width, _clipRect, rect, image->getPixels(), image->getWidth(), image_rect, rgba_set()); break;
                    case BlendMode::Mix:      primitives::TexturedRectangle(_pixels, _width, _clipRect, rect, image->getPixels(), image->getWidth(), image_rect, rgba_mix()); break;
                    case BlendMode::Add:      primitives::TexturedRectangle(_pixels, _width, _clipRect, rect, image->getPixels(), image->getWidth(), image_rect, rgba_add()); break;
                    case BlendMode::Subtract: primitives::TexturedRectangle(_pixels, _width, _clipRect, rect, image->getPixels(), image->getWidth(), image_rect, rgba_sub()); break;
                    case BlendMode::Multiply: primitives::TexturedRectangle(_pixels, _width, _clipRect, rect, image->getPixels(), image->getWidth(), image_rect, rgba_mul()); break;
                    }
                }
                else
                {
                    switch (_blendMode) {
                    case BlendMode::Set:      primitives::TexturedRectangle(_pixels, _width, _clipRect, rect, image->getPixels(), image->getWidth(), image_rect, rgba_set_col(color)); break;
                    case BlendMode::Mix:      primitives::TexturedRectangle(_pixels, _width, _clipRect, rect, image->getPixels(), image->getWidth(), image_rect, rgba_mix_col(color)); break;
                    case BlendMode::Add:      primitives::TexturedRectangle(_pixels, _width, _clipRect, rect, image->getPixels(), image->getWidth(), image_rect, rgba_add_col(color)); break;
                    case BlendMode::Subtract: primitives::TexturedRectangle(_pixels, _width, _clipRect, rect, image->getPixels(), image->getWidth(), image_rect, rgba_sub_col(color)); break;
                    case BlendMode::Multiply: primitives::TexturedRectangle(_pixels, _width, _clipRect, rect, image->getPixels(), image->getWidth(), image_rect, rgba_mul_col(color)); break;
                    }
                }
            }
            else
            {
                Recti rect = Recti(pos, image->getDimensions()).scale(scale);
                Vec2i center_of_rotation = rect.getCenter();

                Vec2i ul = rect.getUpperLeft().rotateBy(rotate, center_of_rotation);
                Vec2i ur = rect.getUpperRight().rotateBy(rotate, center_of_rotation);
                Vec2i lr = rect.getLowerRight().rotateBy(rotate, center_of_rotation);
                Vec2i ll = rect.getLowerLeft().rotateBy(rotate, center_of_rotation);

                drawq(image, image_rect, ul, ur, lr, ll, color);
            }
        }

        //-----------------------------------------------------------------
        void
        Image::drawq(const Image* image, const Vec2i& ul, const Vec2i& ur, const Vec2i& lr, const Vec2i& ll, RGBA color)
        {
            Recti image_rect = Recti(image->getDimensions());
            drawq(image, image_rect, ul, ur, lr, ll, color);
        }

        //-----------------------------------------------------------------
        void
        Image::drawq(const Image* image, const Recti& image_rect, const Vec2i& ul, const Vec2i& ur, const Vec2i& lr, const Vec2i& ll, RGBA color)
        {
            Vec2i pos[4] = { ul, ur, lr, ll };

            if (color == RGBA(255, 255, 255, 255))
            {
                switch (_blendMode) {
                case BlendMode::Set:      primitives::TexturedQuad(_pixels, _width, _clipRect, pos, image->getPixels(), image->getWidth(), image_rect, rgba_set()); break;
                case BlendMode::Mix:      primitives::TexturedQuad(_pixels, _width, _clipRect, pos, image->getPixels(), image->getWidth(), image_rect, rgba_mix()); break;
                case BlendMode::Add:      primitives::TexturedQuad(_pixels, _width, _clipRect, pos, image->getPixels(), image->getWidth(), image_rect, rgba_add()); break;
                case BlendMode::Subtract: primitives::TexturedQuad(_pixels, _width, _clipRect, pos, image->getPixels(), image->getWidth(), image_rect, rgba_sub()); break;
                case BlendMode::Multiply: primitives::TexturedQuad(_pixels, _width, _clipRect, pos, image->getPixels(), image->getWidth(), image_rect, rgba_mul()); break;
                }
            }
            else
            {
                switch (_blendMode) {
                case BlendMode::Set:      primitives::TexturedQuad(_pixels, _width, _clipRect, pos, image->getPixels(), image->getWidth(), image_rect, rgba_set_col(color)); break;
                case BlendMode::Mix:      primitives::TexturedQuad(_pixels, _width, _clipRect, pos, image->getPixels(), image->getWidth(), image_rect, rgba_mix_col(color)); break;
                case BlendMode::Add:      primitives::TexturedQuad(_pixels, _width, _clipRect, pos, image->getPixels(), image->getWidth(), image_rect, rgba_add_col(color)); break;
                case BlendMode::Subtract: primitives::TexturedQuad(_pixels, _width, _clipRect, pos, image->getPixels(), image->getWidth(), image_rect, rgba_sub_col(color)); break;
                case BlendMode::Multiply: primitives::TexturedQuad(_pixels, _width, _clipRect, pos, image->getPixels(), image->getWidth(), image_rect, rgba_mul_col(color)); break;
                }
            }
        }

        //-----------------------------------------------------------------
        void
        Image::drawText(const Font* font, Vec2i pos, const char* text, int len, float scale, RGBA color)
        {
            if (len < 0) {
                len = std::strlen(text);
            }

            int cur_x = pos.x;
            int cur_y = pos.y;

            for (int i = 0; i < len; i++)
            {
                switch (text[i])
                {
                    case ' ':
                    {
                        const Image* space_char_image = font->getCharImage(' ');
                        int space_w = (int)(space_char_image ? space_char_image->getWidth() * scale : 0);
                        cur_x += space_w;
                        break;
                    }
                    case '\t':
                    {
                        const Image* space_char_image = font->getCharImage(' ');
                        int tab_w = (int)(space_char_image ? space_char_image->getWidth() * font->getTabWidth() * scale : 0);
                        if (tab_w > 0) {
                            tab_w = tab_w - ((cur_x - pos.x) % tab_w);
                        }
                        cur_x += tab_w;
                        break;
                    }
                    case '\n':
                    {
                        cur_x = pos.x;
                        cur_y += (int)(font->getMaxCharHeight() * scale);
                        break;
                    }
                    default:
                    {
                        const Image* char_image = font->getCharImage(text[i]);
                        if (char_image)
                        {
                            if (color == RGBA(255, 255, 255, 255))
                            {
                                primitives::TexturedRectangle(
                                    _pixels,
                                    _width,
                                    _clipRect,
                                    Recti(cur_x, cur_y, char_image->getWidth(), char_image->getHeight()).scale(scale),
                                    char_image->getPixels(),
                                    char_image->getWidth(),
                                    Recti(char_image->getDimensions()),
                                    rgba_mix()
                                );
                            }
                            else
                            {
                                primitives::TexturedRectangle(
                                    _pixels,
                                    _width,
                                    _clipRect,
                                    Recti(cur_x, cur_y, char_image->getWidth(), char_image->getHeight()).scale(scale),
                                    char_image->getPixels(),
                                    char_image->getWidth(),
                                    Recti(char_image->getDimensions()),
                                    rgba_mix_col(color)
                                );
                            }
                            cur_x += (int)(char_image->getWidth() * scale);
                        }
                        break;
                    }
                }
            }
        }

        //-----------------------------------------------------------------
        void
        Image::drawWindow(const WindowSkin* windowSkin, Recti windowRect, int opacity)
        {
            if (!windowRect.isValid())
            {
                return;
            }

            // for brevity
            int x1 = windowRect.ul.x;
            int y1 = windowRect.ul.y;
            int x2 = windowRect.lr.x;
            int y2 = windowRect.lr.y;

            const Image* tlBorder = windowSkin->getBorderImage(WindowSkin::TopLeftBorder);
            const Image* trBorder = windowSkin->getBorderImage(WindowSkin::TopRightBorder);
            const Image* brBorder = windowSkin->getBorderImage(WindowSkin::BottomRightBorder);
            const Image* blBorder = windowSkin->getBorderImage(WindowSkin::BottomLeftBorder);

            // draw background
            if (!windowRect.isEmpty())
            {
                RGBA tlColor = windowSkin->getBgColor(WindowSkin::TopLeftBgColor);
                RGBA trColor = windowSkin->getBgColor(WindowSkin::TopRightBgColor);
                RGBA brColor = windowSkin->getBgColor(WindowSkin::BottomRightBgColor);
                RGBA blColor = windowSkin->getBgColor(WindowSkin::BottomLeftBgColor);

                // apply opacity
                tlColor.alpha = (tlColor.alpha * opacity) / 255;
                trColor.alpha = (trColor.alpha * opacity) / 255;
                brColor.alpha = (brColor.alpha * opacity) / 255;
                blColor.alpha = (blColor.alpha * opacity) / 255;

                primitives::Rectangle(
                    _pixels,
                    _width,
                    _clipRect,
                    true,
                    windowRect,
                    tlColor,
                    trColor,
                    brColor,
                    blColor,
                    rgba_mix()
                );
            }

            // draw top left edge
            primitives::TexturedRectangle(
                _pixels,
                _width,
                _clipRect,
                Vec2i(x1 - tlBorder->getWidth(), y1 - tlBorder->getHeight()),
                tlBorder->getPixels(),
                tlBorder->getWidth(),
                Recti(tlBorder->getDimensions()),
                rgba_mix()
            );

            // draw top right edge
            primitives::TexturedRectangle(
                _pixels,
                _width,
                _clipRect,
                Vec2i(x2 + 1, y1 - trBorder->getHeight()),
                trBorder->getPixels(),
                trBorder->getWidth(),
                Recti(trBorder->getDimensions()),
                rgba_mix()
            );

            // draw bottom right edge
            primitives::TexturedRectangle(
                _pixels,
                _width,
                _clipRect,
                Vec2i(x2 + 1, y2 + 1 ),
                brBorder->getPixels(),
                brBorder->getWidth(),
                Recti(brBorder->getDimensions()),
                rgba_mix()
            );

            // draw bottom left edge
            primitives::TexturedRectangle(
                _pixels,
                _width,
                _clipRect,
                Vec2i(x1 - blBorder->getWidth(), y2 + 1 ),
                blBorder->getPixels(),
                blBorder->getWidth(),
                Recti(blBorder->getDimensions()),
                rgba_mix()
            );

            // draw top and bottom borders
            if (windowRect.getWidth() > 0)
            {
                const Image* tBorder  = windowSkin->getBorderImage(WindowSkin::TopBorder);
                const Image* bBorder  = windowSkin->getBorderImage(WindowSkin::BottomBorder);

                int i;

                // draw top border
                i = x1;
                while ((x2 - i) + 1 >= tBorder->getWidth()) {
                    primitives::TexturedRectangle(
                        _pixels,
                        _width,
                        _clipRect,
                        Vec2i(i, y1 - tBorder->getHeight()),
                        tBorder->getPixels(),
                        tBorder->getWidth(),
                        Recti(tBorder->getDimensions()),
                        rgba_mix()
                    );
                    i += tBorder->getWidth();
                }
                if ((x2 - i) + 1 > 0) {
                    primitives::TexturedRectangle(
                        _pixels,
                        _width,
                        _clipRect,
                        Vec2i(i, y1 - tBorder->getHeight()),
                        tBorder->getPixels(),
                        tBorder->getWidth(),
                        Recti(0, 0, (x2 - i) + 1, tBorder->getHeight()),
                        rgba_mix()
                    );
                }

                // draw bottom border
                i = x1;
                while ((x2 - i) + 1 >= bBorder->getWidth()) {
                    primitives::TexturedRectangle(
                        _pixels,
                        _width,
                        _clipRect,
                        Vec2i(i, y2 + 1),
                        bBorder->getPixels(),
                        bBorder->getWidth(),
                        Recti(bBorder->getDimensions()),
                        rgba_mix()
                    );
                    i += bBorder->getWidth();
                }
                if ((x2 - i) + 1 > 0) {
                    primitives::TexturedRectangle(
                        _pixels,
                        _width,
                        _clipRect,
                        Vec2i(i, y2 + 1),
                        bBorder->getPixels(),
                        bBorder->getWidth(),
                        Recti(0, 0, (x2 - i) + 1, bBorder->getHeight()),
                        rgba_mix()
                    );
                }
            }

            // draw left and right borders
            if (windowRect.getHeight() > 0)
            {
                const Image* lBorder  = windowSkin->getBorderImage(WindowSkin::LeftBorder);
                const Image* rBorder  = windowSkin->getBorderImage(WindowSkin::RightBorder);

                int i;

                // draw left border
                i = y1;
                while ((y2 - i) + 1 >= lBorder->getHeight()) {
                    primitives::TexturedRectangle(
                        _pixels,
                        _width,
                        _clipRect,
                        Vec2i(x1 - lBorder->getWidth(), i),
                        lBorder->getPixels(),
                        lBorder->getWidth(),
                        Recti(lBorder->getDimensions()),
                        rgba_mix()
                    );
                    i += lBorder->getHeight();
                }
                if ((y2 - i) + 1 > 0) {
                    primitives::TexturedRectangle(
                        _pixels,
                        _width,
                        _clipRect,
                        Vec2i(x1 - lBorder->getWidth(), i),
                        lBorder->getPixels(),
                        lBorder->getWidth(),
                        Recti(0, 0, lBorder->getWidth(), (y2 - i) + 1),
                        rgba_mix()
                    );
                }

                // draw right border
                i = y1;
                while ((y2 - i) + 1 >= rBorder->getHeight()) {
                    primitives::TexturedRectangle(
                        _pixels,
                        _width,
                        _clipRect,
                        Vec2i(x2 + 1, i),
                        rBorder->getPixels(),
                        rBorder->getWidth(),
                        Recti(rBorder->getDimensions()),
                        rgba_mix()
                    );
                    i += rBorder->getHeight();
                }
                if ((y2 - i) + 1 > 0) {
                    primitives::TexturedRectangle(
                        _pixels,
                        _width,
                        _clipRect,
                        Vec2i(x2 + 1, i),
                        rBorder->getPixels(),
                        rBorder->getWidth(),
                        Recti(0, 0, rBorder->getWidth(), (y2 - i) + 1),
                        rgba_mix()
                    );
                }
            }
        }

    } // namespace graphics
} // namespace rpgss
