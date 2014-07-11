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

#ifndef RPGSS_GRAPHICS_PRIMITIVES_HPP_INCLUDED
#define RPGSS_GRAPHICS_PRIMITIVES_HPP_INCLUDED

#include "../common/types.hpp"
#include "../core/Vec2.hpp"
#include "../core/Rect.hpp"
#include "RGBA.hpp"


namespace rpgss {
    namespace graphics {
        namespace primitives {

            //-----------------------------------------------------------------
            template<typename T>
            T minmax(T t, T min, T max)
            {
                return (t < min) ? min : ((t > max) ? max : t);
            }

            //-----------------------------------------------------------------
            template<typename dstT, typename renderT>
            void Point(
                dstT*        dstPixels,
                int          dstPitch,
                const core::Recti& dstClipRect,
                const core::Vec2i& pos,
                RGBA         color,
                renderT      renderer)
            {
                if (dstClipRect.isEmpty()) {
                    return;
                }

                if (dstClipRect.contains(pos)) {
                    renderer(dstPixels + dstPitch * pos.y + pos.x, &color);
                }
            }

            //-----------------------------------------------------------------
            static bool clip_line_test(const float& p, const float& q, float& u1, float& u2)
            {
                float r;
                if (p < 0.0) {
                    r = q / p;
                    if (r > u2) {
                        return false;
                    } else if (r > u1) {
                        u1 = r;
                    }
                } else if (p > 0.0) {
                    r = q / p;
                    if (r < u1) {
                        return false;
                    } else if (r < u2) {
                        u2 = r;
                    }
                }
                return true;
            }

            static bool clip_line(int& x1, int& y1, int& x2, int& y2, float& u1, float& u2, const core::Recti& clipRect)
            {
                float dx = (float)(x2 - x1);
                float dy;

                if(clip_line_test(-dx, (float)(x1 - clipRect.ul.x), u1, u2)) {
                    if(clip_line_test(dx, (float)(clipRect.lr.x - x1), u1, u2)) {
                        dy = (float)(y2 - y1);

                        if(clip_line_test(-dy, (float)(y1 - clipRect.ul.y), u1, u2)) {
                            if(clip_line_test(dy, (float)(clipRect.lr.y - y1), u1, u2)) {
                                if (u2 < 1.0) {
                                    x2 = (int)(x1 + u2 * dx + 0.5);
                                    y2 = (int)(y1 + u2 * dy + 0.5);
                                }

                                if (u1 > 0.0) {
                                    x1 = (int)(x1 + u1 * dx + 0.5);
                                    y1 = (int)(y1 + u1 * dy + 0.5);
                                }

                                return true;
                            }
                        }
                    }
                }

                return false;
            }

            //-----------------------------------------------------------------
            inline bool is_line_clipped(int x1, int y1, int x2, int y2, const core::Recti& clipRect)
            {
                // return true if any end point lies outside of the clipping rectangle
                return (x1 < clipRect.ul.x || x2 < clipRect.ul.x ||
                        x1 > clipRect.lr.x || x2 > clipRect.lr.x ||
                        y1 < clipRect.ul.y || y2 < clipRect.ul.y ||
                        y1 > clipRect.lr.y || y2 > clipRect.lr.y);
            }

            //-----------------------------------------------------------------
            template<typename dstT, typename renderT>
            __attribute__((__noinline__))
            void Line(
                dstT*        dstPixels,
                int          dstPitch,
                core::Recti  dstClipRect,
                core::Vec2i  startPos,
                core::Vec2i  endPos,
                RGBA         color,
                renderT      renderer)
            {
                if (dstClipRect.isEmpty()) {
                    return;
                }

                int x1 = startPos.x;
                int y1 = startPos.y;
                int x2 = endPos.x;
                int y2 = endPos.y;

                if (startPos == endPos)
                {
                    // single pixel line
                    if (dstClipRect.contains(startPos)) {
                        dstT* dptr = dstPixels + y1 * dstPitch + x1;
                        renderer(dptr, &color);
                    }
                }
                else if (y1 == y2 || x1 == x2)
                {
                    // horizontal or vertical line
                    dstT* dptr = 0;
                    int dinc, i1, i2;

                    if (y1 == y2) {
                        if (y1 < dstClipRect.ul.y || y1 > dstClipRect.lr.y) {
                            return; // fully clipped
                        }
                        i1 = minmax(x1, dstClipRect.ul.x, dstClipRect.lr.x);
                        i2 = minmax(x2, dstClipRect.ul.x, dstClipRect.lr.x);
                        dptr = dstPixels + y1 * dstPitch + i1;
                        dinc = 1;
                    } else {
                        if (x1 < dstClipRect.ul.x || x1 > dstClipRect.lr.x) {
                            return; // fully clipped
                        }
                        i1 = minmax(y1, dstClipRect.ul.y, dstClipRect.lr.y);
                        i2 = minmax(y2, dstClipRect.ul.y, dstClipRect.lr.y);
                        dptr = dstPixels + i1 * dstPitch + x1;
                        dinc = dstPitch;
                    }

                    int ix = 2;

                    if (i1 <= i2) {
                        ix += i2 - i1;
                    } else {
                        ix += i1 - i2;
                        dinc = -dinc;
                    }

                    while (--ix) {
                        renderer(dptr, &color);
                        dptr += dinc;
                    }

                }
                else
                {
                    // other lines
                    if (is_line_clipped(x1, y1, x2, y2, dstClipRect)) {
                        float u1 = 0.0, u2 = 1.0;
                        if (!clip_line(x1, y1, x2, y2, u1, u2, dstClipRect)) {
                            return;
                        }
                    }

                    dstT* dptr = dstPixels + y1 * dstPitch + x1;

                    int dx = std::abs(x2 - x1);
                    int dy = std::abs(y2 - y1);
                    int xinc1, xinc2, yinc1, yinc2;
                    int num, den, numadd, numpix;

                    if (x2 >= x1) {
                        xinc1 = 1;
                        xinc2 = 1;
                    } else {
                        xinc1 = -1;
                        xinc2 = -1;
                    }

                    if (y2 >= y1) {
                        yinc1 = dstPitch;
                        yinc2 = dstPitch;
                    } else {
                        yinc1 = -dstPitch;
                        yinc2 = -dstPitch;
                    }

                    if (dx >= dy) {
                        xinc1  = 0;
                        yinc2  = 0;
                        den    = dx;
                        num    = dx / 2;
                        numadd = dy;
                        numpix = dx;
                    } else {
                        xinc2  = 0;
                        yinc1  = 0;
                        den    = dy;
                        num    = dy / 2;
                        numadd = dx;
                        numpix = dy;
                    }

                    for (int i = 0; i < numpix; ++i) {
                        renderer(dptr, &color);
                        num += numadd;
                        if (num >= den) {
                            num -= den;
                            dptr += xinc1;
                            dptr += yinc1;
                        }
                        dptr += xinc2;
                        dptr += yinc2;
                    }
                }
            }

            //-----------------------------------------------------------------
            inline void clip_color_range(float u1, float u2, RGBA& c1, RGBA& c2)
            {
                RGBA ct = c1;

                c1.red   = (u8)(c1.red   + u1 * (c2.red   - c1.red  ));
                c1.green = (u8)(c1.green + u1 * (c2.green - c1.green));
                c1.blue  = (u8)(c1.blue  + u1 * (c2.blue  - c1.blue ));
                c1.alpha = (u8)(c1.alpha + u1 * (c2.alpha - c1.alpha));

                c2.red   = (u8)(ct.red   + u2 * (c2.red   - ct.red  ));
                c2.green = (u8)(ct.green + u2 * (c2.green - ct.green));
                c2.blue  = (u8)(ct.blue  + u2 * (c2.blue  - ct.blue ));
                c2.alpha = (u8)(ct.alpha + u2 * (c2.alpha - ct.alpha));
            }

            //-----------------------------------------------------------------
            template<typename dstT, typename renderT>
            __attribute__((__noinline__))
            void Line(
                dstT*        dstPixels,
                int          dstPitch,
                core::Recti  dstClipRect,
                core::Vec2i  startPos,
                core::Vec2i  endPos,
                RGBA         startColor,
                RGBA         endColor,
                renderT      renderer)
            {
                if (dstClipRect.isEmpty()) {
                    return;
                }

                if (startColor == endColor) {
                    // fall back on simpler algorithm
                    Line(dstPixels, dstPitch, dstClipRect, startPos, endPos, startColor, renderer);
                    return;
                }

                int x1 = startPos.x;
                int y1 = startPos.y;
                int x2 = endPos.x;
                int y2 = endPos.y;

                RGBA  c[2] = {startColor, endColor};

                if (startPos == endPos) { // single pixel line

                    if (dstClipRect.contains(startPos)) {
                        dstT* dptr = dstPixels + y1 * dstPitch + x1;
                        renderer(dptr, &startColor);
                    }

                } else if (y1 == y2 || x1 == x2) { // horizontal or vertical lines

                    dstT* dptr = 0;
                    int dinc, itemp, idelta, i1, i2;

                    if (y1 == y2) {
                        i1     = minmax(x1, dstClipRect.ul.x, dstClipRect.lr.x);
                        i2     = minmax(x2, dstClipRect.ul.x, dstClipRect.lr.x);
                        itemp  = x1;
                        idelta = x2 - x1;
                        dptr   = dstPixels + y1 * dstPitch + i1;
                        dinc   = 1;
                    } else {
                        i1     = minmax(y1, dstClipRect.ul.y, dstClipRect.lr.y);
                        i2     = minmax(y2, dstClipRect.ul.y, dstClipRect.lr.y);
                        itemp  = y1;
                        idelta = y2 - y1;
                        dptr   = dstPixels + i1 * dstPitch + x1;
                        dinc   = dstPitch;
                    }

                    int ix = 2 + std::abs(i2 - i1);

                    if (i1 > i2) {
                        dinc = -dinc;
                    }

                    if (is_line_clipped(x1, y1, x2, y2, dstClipRect)) {
                        float u1 = (idelta == 0) ? 0.0f : (i1 - itemp) / (float)idelta;
                        float u2 = (idelta == 0) ? 0.0f : (i2 - itemp) / (float)idelta;
                        clip_color_range(u1, u2, c[0], c[1]);
                    }

                    // fixed-point variables for color interpolation (20.12 notation)
                    i32 r = c[0].red   << 12;
                    i32 g = c[0].green << 12;
                    i32 b = c[0].blue  << 12;
                    i32 a = c[0].alpha << 12;

                    // (ix - 1) is always > 0
                    i32 r_step = (i32)(((c[1].red   - c[0].red)   / (float)(ix - 1)) * 4096.0);
                    i32 g_step = (i32)(((c[1].green - c[0].green) / (float)(ix - 1)) * 4096.0);
                    i32 b_step = (i32)(((c[1].blue  - c[0].blue)  / (float)(ix - 1)) * 4096.0);
                    i32 a_step = (i32)(((c[1].alpha - c[0].alpha) / (float)(ix - 1)) * 4096.0);

                    while (--ix) {
                        renderer(dptr, r >> 12, g >> 12, b >> 12, a >> 12);

                        dptr += dinc;

                        r += r_step;
                        g += g_step;
                        b += b_step;
                        a += a_step;
                    }

                } else { // other lines

                    if (is_line_clipped(x1, y1, x2, y2, dstClipRect)) {
                        float u1 = 0.0, u2 = 1.0;

                        if (!clip_line(x1, y1, x2, y2, u1, u2, dstClipRect)) {
                            return;
                        }

                        clip_color_range(u1, u2, c[0], c[1]);
                    }

                    dstT* dptr = dstPixels + y1 * dstPitch + x1;

                    int dx = std::abs(x2 - x1);
                    int dy = std::abs(y2 - y1);

                    int xinc1, xinc2, yinc1, yinc2;
                    int num, den, numadd, numpix;

                    if (x2 >= x1) {
                        xinc1 = 1;
                        xinc2 = 1;
                    } else {
                        xinc1 = -1;
                        xinc2 = -1;
                    }

                    if (y2 >= y1) {
                        yinc1 = dstPitch;
                        yinc2 = dstPitch;
                    } else {
                        yinc1 = -dstPitch;
                        yinc2 = -dstPitch;
                    }

                    if (dx >= dy) {
                        xinc1 = 0;
                        yinc2 = 0;
                        den = dx;
                        num = dx / 2;
                        numadd = dy;
                        numpix = dx + 1;
                    } else {
                        xinc2 = 0;
                        yinc1 = 0;
                        den = dy;
                        num = dy / 2;
                        numadd = dx;
                        numpix = dy + 1;
                    }

                    // fixed-point variables for color interpolation (20.12 notation)
                    u32 r = c[0].red   << 12;
                    u32 g = c[0].green << 12;
                    u32 b = c[0].blue  << 12;
                    u32 a = c[0].alpha << 12;

                    // numpix is always > 0
                    i32 r_step = (i32)(((c[1].red   - c[0].red)   / (float)numpix) * 4096.0);
                    i32 g_step = (i32)(((c[1].green - c[0].green) / (float)numpix) * 4096.0);
                    i32 b_step = (i32)(((c[1].blue  - c[0].blue)  / (float)numpix) * 4096.0);
                    i32 a_step = (i32)(((c[1].alpha - c[0].alpha) / (float)numpix) * 4096.0);

                    for (int i = 0; i < numpix; ++i) {
                        renderer(dptr, r >> 12, g >> 12, b >> 12, a >> 12);

                        num += numadd;
                        if (num >= den) {
                            num -= den;
                            dptr += xinc1;
                            dptr += yinc1;
                        }

                        dptr += xinc2;
                        dptr += yinc2;

                        r += r_step;
                        g += g_step;
                        b += b_step;
                        a += a_step;
                    }
                }
            }

            //-----------------------------------------------------------------
            template<typename dstT, typename renderT>
            __attribute__((__noinline__))
            void Rectangle(
                dstT*        dstPixels,
                int          dstPitch,
                core::Recti  dstClipRect,
                bool         fill,
                core::Recti  rect,
                RGBA         color,
                renderT      renderer)
            {
                if (dstClipRect.isEmpty() || rect.isEmpty()) {
                    return;
                }

                if (!fill)
                {
                    // rectangle outline
                    Line(dstPixels, dstPitch, dstClipRect, rect.getUpperLeft(),  rect.getUpperRight(), color, renderer);
                    Line(dstPixels, dstPitch, dstClipRect, rect.getUpperRight(), rect.getLowerRight(), color, renderer);
                    Line(dstPixels, dstPitch, dstClipRect, rect.getLowerRight(), rect.getLowerLeft(),  color, renderer);
                    Line(dstPixels, dstPitch, dstClipRect, rect.getLowerLeft(),  rect.getUpperLeft(),  color, renderer);
                }
                else
                {
                    // filled rectangle
                    core::Recti drct = dstClipRect.getIntersection(rect);
                    if (drct.isEmpty()) {
                        return;
                    }

                    dstT*     dptr = dstPixels + drct.getY() * dstPitch + drct.getX();
                    const int dinc = dstPitch - drct.getWidth();

                    int iy = drct.getHeight();
                    while (iy > 0) {
                        int ix = drct.getWidth();
                        while (ix > 0) {
                            renderer(dptr, &color);
                            ++dptr;
                            --ix;
                        }
                        dptr += dinc;
                        --iy;
                    }
                }
            }

            //-----------------------------------------------------------------
            inline void clip_color_range(float u, RGBA& ul, const RGBA& ur, RGBA& ll, const RGBA& lr)
            {
                if (u == 0.0) {
                    return;
                }

                ul.red   = (u8)(ul.red   + u * (ur.red   - ul.red));
                ul.green = (u8)(ul.green + u * (ur.green - ul.green));
                ul.blue  = (u8)(ul.blue  + u * (ur.blue  - ul.blue));
                ul.alpha = (u8)(ul.alpha + u * (ur.alpha - ul.alpha));

                ll.red   = (u8)(ll.red   + u * (lr.red   - ll.red));
                ll.green = (u8)(ll.green + u * (lr.green - ll.green));
                ll.blue  = (u8)(ll.blue  + u * (lr.blue  - ll.blue));
                ll.alpha = (u8)(ll.alpha + u * (lr.alpha - ll.alpha));
            }

            //-----------------------------------------------------------------
            template<typename dstT, typename renderT>
            __attribute__((__noinline__))
            void Rectangle(
                dstT*        dstPixels,
                int          dstPitch,
                core::Recti  dstClipRect,
                bool         fill,
                core::Recti  rect,
                RGBA         ulColor,
                RGBA         urColor,
                RGBA         lrColor,
                RGBA         llColor,
                renderT      renderer)
            {
                if (ulColor == urColor && ulColor == lrColor && ulColor == llColor) {
                    // fall back on simpler algorithm
                    Rectangle(dstPixels, dstPitch, dstClipRect, fill, rect, ulColor, renderer);
                    return;
                }

                if (dstClipRect.isEmpty() || rect.isEmpty()) {
                    return;
                }

                if (!fill)
                {
                    // rectangle outline
                    Line(dstPixels, dstPitch, dstClipRect, rect.getUpperLeft(),  rect.getUpperRight(), ulColor, urColor, renderer);
                    Line(dstPixels, dstPitch, dstClipRect, rect.getUpperRight(), rect.getLowerRight(), urColor, lrColor, renderer);
                    Line(dstPixels, dstPitch, dstClipRect, rect.getLowerRight(), rect.getLowerLeft(),  lrColor, llColor, renderer);
                    Line(dstPixels, dstPitch, dstClipRect, rect.getLowerLeft(),  rect.getUpperLeft(),  llColor, ulColor, renderer);
                }
                else
                {
                    // filled rectangle
                    core::Recti drct = dstClipRect.getIntersection(rect);
                    if (drct.isEmpty()) {
                        return;
                    }

                    RGBA c[4] = {ulColor, urColor, lrColor, llColor};

                    clip_color_range((drct.getX() - rect.getX()) / (float)rect.getWidth(),   c[0], c[1], c[3], c[2]);
                    clip_color_range((drct.getY() - rect.getY()) / (float)rect.getHeight(),  c[1], c[2], c[0], c[3]);
                    clip_color_range(    1.0 - (drct.getWidth()  / (float)rect.getWidth()),  c[2], c[3], c[1], c[0]);
                    clip_color_range(    1.0 - (drct.getHeight() / (float)rect.getHeight()), c[3], c[0], c[2], c[1]);

                    i32 l_r = c[0].red   << 12;
                    i32 l_g = c[0].green << 12;
                    i32 l_b = c[0].blue  << 12;
                    i32 l_a = c[0].alpha << 12;

                    i32 r_r = c[1].red   << 12;
                    i32 r_g = c[1].green << 12;
                    i32 r_b = c[1].blue  << 12;
                    i32 r_a = c[1].alpha << 12;

                    i32 step_l_r = ((c[3].red   - c[0].red)   << 12) / drct.getHeight();
                    i32 step_l_g = ((c[3].green - c[0].green) << 12) / drct.getHeight();
                    i32 step_l_b = ((c[3].blue  - c[0].blue)  << 12) / drct.getHeight();
                    i32 step_l_a = ((c[3].alpha - c[0].alpha) << 12) / drct.getHeight();

                    i32 step_r_r = ((c[2].red   - c[1].red)   << 12) / drct.getHeight();
                    i32 step_r_g = ((c[2].green - c[1].green) << 12) / drct.getHeight();
                    i32 step_r_b = ((c[2].blue  - c[1].blue)  << 12) / drct.getHeight();
                    i32 step_r_a = ((c[2].alpha - c[1].alpha) << 12) / drct.getHeight();

                    dstT*     dptr = dstPixels + drct.getY() * dstPitch + drct.getX();
                    const int dinc = dstPitch - drct.getWidth();

                    int iy = drct.getHeight();
                    while (iy > 0) {
                        i32 r = l_r;
                        i32 g = l_g;
                        i32 b = l_b;
                        i32 a = l_a;

                        i32 step_r = (r_r - l_r) / drct.getWidth();
                        i32 step_g = (r_g - l_g) / drct.getWidth();
                        i32 step_b = (r_b - l_b) / drct.getWidth();
                        i32 step_a = (r_a - l_a) / drct.getWidth();

                        int ix = drct.getWidth();
                        while (ix > 0) {
                            renderer(dptr, r >> 12, g >> 12, b >> 12, a >> 12);

                            ++dptr;

                            r += step_r;
                            g += step_g;
                            b += step_b;
                            a += step_a;

                            --ix;
                        }

                        dptr += dinc;

                        l_r += step_l_r;
                        l_g += step_l_g;
                        l_b += step_l_b;
                        l_a += step_l_a;

                        r_r += step_r_r;
                        r_g += step_r_g;
                        r_b += step_r_b;
                        r_a += step_r_a;

                        --iy;
                    }
                }
            }

            //-----------------------------------------------------------------
            template<typename dstT, typename renderT>
            __attribute__((__noinline__))
            void Triangle(
                dstT*        dstPixels,
                int          dstPitch,
                core::Recti  dstClipRect,
                core::Vec2i  vertices[4],
                RGBA         color,
                renderT      renderer)
            {
                // TODO
            }

            //-----------------------------------------------------------------
            template<typename dstT, typename renderT>
            __attribute__((__noinline__))
            void Triangle(
                dstT*        dstPixels,
                int          dstPitch,
                core::Recti  dstClipRect,
                core::Vec2i  vertices[4],
                RGBA         colors[3],
                renderT      renderer)
            {
                // TODO
            }

            //-----------------------------------------------------------------
            template<typename dstT, typename renderT>
            __attribute__((__noinline__))
            void Circle(
                dstT*        dstPixels,
                int          dstPitch,
                core::Recti  dstClipRect,
                bool         fill,
                core::Vec2i  center,
                int          radius,
                RGBA         color,
                renderT      renderer)
            {
                int r = radius;
                int x = center.x;
                int y = center.y;

                core::Recti bbox = core::Recti(x - r, y - r, r * 2, r * 2);

                if (r <= 0 || dstClipRect.isEmpty() || dstClipRect.getIntersection(bbox).isEmpty()) {
                    return;
                }

                if (!fill)
                {
                    // circle outline
                    int f     = 1 - r;
                    int ddF_x = 0;
                    int ddF_y = -2 * r;

                    int ix = 0;
                    int iy = r;

                    dstT* tl = dstPixels + (y - r)     * dstPitch + (x);
                    dstT* tr = dstPixels + (y - r)     * dstPitch + (x - 1);
                    dstT* bl = dstPixels + (y + r - 1) * dstPitch + (x);
                    dstT* br = dstPixels + (y + r - 1) * dstPitch + (x - 1);
                    dstT* lt = dstPixels + (y)         * dstPitch + (x - r);
                    dstT* lb = dstPixels + (y - 1)     * dstPitch + (x - r);
                    dstT* rt = dstPixels + (y)         * dstPitch + (x + r - 1);
                    dstT* rb = dstPixels + (y - 1)     * dstPitch + (x + r - 1);

                    while (ix < iy) {
                        ix++;
                        tl--;
                        tr++;
                        bl--;
                        br++;

                        lt -= dstPitch;
                        lb += dstPitch;
                        rt -= dstPitch;
                        rb += dstPitch;

                        ddF_x += 2;
                        f     += ddF_x + 1;

                        if (dstClipRect.contains(x - ix,     y - iy    )) renderer(tl, &color);
                        if (dstClipRect.contains(x + ix - 1, y - iy    )) renderer(tr, &color);
                        if (dstClipRect.contains(x - ix,     y + iy - 1)) renderer(bl, &color);
                        if (dstClipRect.contains(x + ix - 1, y + iy - 1)) renderer(br, &color);

                        if (ix != iy) {
                            if (dstClipRect.contains(x - iy,     y - ix    )) renderer(lt, &color);
                            if (dstClipRect.contains(x + iy - 1, y - ix    )) renderer(rt, &color);
                            if (dstClipRect.contains(x - iy,     y + ix - 1)) renderer(lb, &color);
                            if (dstClipRect.contains(x + iy - 1, y + ix - 1)) renderer(rb, &color);
                        }

                        if (f >= 0) {
                            iy--;
                            lt++;
                            lb++;
                            rt--;
                            rb--;

                            tl += dstPitch;
                            tr += dstPitch;
                            bl -= dstPitch;
                            br -= dstPitch;

                            ddF_y += 2;
                            f     += ddF_y;
                        }
                    }
                }
                else
                {
                    // filled circle
                    int f     = 1 - r;
                    int ddF_x = 0;
                    int ddF_y = -2 * r;

                    int ix = 0;
                    int iy = r;
                    int clip_l;
                    int clip_r;

                    while (ix < iy) {
                        ix++;
                        ddF_x += 2;
                        f     += ddF_x + 1;

                        clip_l = std::max(x - iy,     dstClipRect.ul.x);
                        clip_r = std::min(x + iy - 1, dstClipRect.lr.x);

                        // top half - bottom
                        if (y - ix >= dstClipRect.ul.y && y - ix <= dstClipRect.lr.y) {
                            dstT* dst = dstPixels + (y - ix) * dstPitch + clip_l;
                            for (int i = 0, j = clip_r + 1 - clip_l; i < j; ++i, ++dst) {
                                renderer(dst, &color);
                            }
                        }

                        // bottom half - top
                        if (y + ix - 1 >= dstClipRect.ul.y && y + ix - 1 <= dstClipRect.lr.y) {
                            dstT* dst = dstPixels + (y + ix - 1) * dstPitch + clip_l;
                            for (int i = 0, j = clip_r + 1 - clip_l; i < j; ++i, ++dst) {
                                renderer(dst, &color);
                            }
                        }

                        if (f >= 0) {
                            if (ix != iy) {
                                clip_l = std::max(x - ix,     dstClipRect.ul.x);
                                clip_r = std::min(x + ix - 1, dstClipRect.lr.x);

                                // top half - top
                                if (y - iy >= dstClipRect.ul.y && y - iy <= dstClipRect.lr.y) {
                                    dstT* dst = dstPixels + (y - iy) * dstPitch + clip_l;
                                    for (int i = 0, j = clip_r + 1 - clip_l; i < j; ++i, ++dst) {
                                        renderer(dst, &color);
                                    }
                                }

                                // bottom half - bottom
                                if (y + iy - 1 >= dstClipRect.ul.y && y + iy - 1 <= dstClipRect.lr.y) {
                                    dstT* dst = dstPixels + (y + iy - 1) * dstPitch + clip_l;
                                    for (int i = 0, j = clip_r + 1 - clip_l; i < j; ++i, ++dst) {
                                        renderer(dst, &color);
                                    }
                                }
                            }

                            iy--;
                            ddF_y += 2;
                            f     += ddF_y;
                        }
                    }
                }
            }

            //-----------------------------------------------------------------
            template<typename dstT, typename renderT>
            __attribute__((__noinline__))
            void Circle(
                dstT*        dstPixels,
                int          dstPitch,
                core::Recti  dstClipRect,
                bool         fill,
                core::Vec2i  center,
                int          radius,
                RGBA         innerColor,
                RGBA         outerColor,
                renderT      renderer)
            {
                if (outerColor == innerColor || !fill) {
                    // fall back on simpler algorithm
                    Circle(dstPixels, dstPitch, dstClipRect, fill, center, radius, outerColor, renderer);
                    return;
                }

                int r = radius;
                int x = center.x;
                int y = center.y;

                if (r <= 0 ||
                    dstClipRect.isEmpty() ||
                    dstClipRect.getIntersection(core::Recti(x - r, y - r, r * 2, r * 2)).isEmpty())
                {
                    return;
                }

                RGBA c;

                int ix = 1;
                int iy = r;
                int n;

                float dr = outerColor.red   - innerColor.red;
                float dg = outerColor.green - innerColor.green;
                float db = outerColor.blue  - innerColor.blue;
                float da = outerColor.alpha - innerColor.alpha;

                float dist, u;

                const float PI_H = 3.14159f / 2.0f;
                const float RR   = r * r;

                while (ix <= iy) {
                    n = iy + 1;
                    while (--n >= ix) {
                        dist = std::sqrt((float)(ix*ix + n*n));

                        if (dist > r) {
                            dist = r;
                        }

                        u = std::sin((1.0f - dist / r) * PI_H);

                        c.red   = (u8)(outerColor.red   - dr * u);
                        c.green = (u8)(outerColor.green - dg * u);
                        c.blue  = (u8)(outerColor.blue  - db * u);
                        c.alpha = (u8)(outerColor.alpha - da * u);

                        if (dstClipRect.contains(x + ix - 1, y - n    )) { renderer(dstPixels + (y - n    ) * dstPitch + (x + ix - 1), &c); }
                        if (dstClipRect.contains(x - ix,     y - n    )) { renderer(dstPixels + (y - n    ) * dstPitch + (x - ix    ), &c); }
                        if (dstClipRect.contains(x + ix - 1, y + n - 1)) { renderer(dstPixels + (y + n - 1) * dstPitch + (x + ix - 1), &c); }
                        if (dstClipRect.contains(x - ix,     y + n - 1)) { renderer(dstPixels + (y + n - 1) * dstPitch + (x - ix    ), &c); }

                        if (ix != n) {
                            if (dstClipRect.contains(x + n - 1, y - ix    )) { renderer(dstPixels + (y - ix    ) * dstPitch + (x + n - 1), &c); }
                            if (dstClipRect.contains(x - n,     y - ix    )) { renderer(dstPixels + (y - ix    ) * dstPitch + (x - n    ), &c); }
                            if (dstClipRect.contains(x + n - 1, y + ix - 1)) { renderer(dstPixels + (y + ix - 1) * dstPitch + (x + n - 1), &c); }
                            if (dstClipRect.contains(x - n,     y + ix - 1)) { renderer(dstPixels + (y + ix - 1) * dstPitch + (x - n    ), &c); }
                        }
                    }

                    ++ix;

                    if (std::fabs(ix*ix + iy*iy - RR) > std::fabs(ix*ix + (iy-1)*(iy-1) - RR)) {
                        --iy;
                    }
                }
            }

            //-----------------------------------------------------------------
            template<typename dstT, typename srcT, typename renderT>
            __attribute__((__noinline__))
            void TexturedRectangle(
                dstT*        dstPixels,
                int          dstPitch,
                core::Recti  dstClipRect,
                core::Vec2i  dstPos,
                const srcT*  srcPixels,
                int          srcPitch,
                core::Recti  srcRect,
                renderT      renderer)
            {
                if (dstClipRect.isEmpty() || srcRect.isEmpty()) {
                    return;
                }

                core::Recti drct = core::Recti(dstPos, srcRect.getDimensions()).getIntersection(dstClipRect);
                if (drct.isEmpty()) {
                    return;
                }

                core::Recti srct(srcRect.getPosition() + (drct.getPosition() - dstPos), drct.getDimensions());

                const int dinc = dstPitch - drct.getWidth();
                dstT*     dptr = dstPixels + (drct.getY() * dstPitch) + drct.getX();

                const int   sinc = srcPitch - srct.getWidth();
                const srcT* sptr = srcPixels + (srct.getY() * srcPitch) + srct.getX();

                int iy = drct.getHeight();
                while (iy > 0) {
                    int ix = drct.getWidth();
                    while (ix > 0) {
                        renderer(dptr, sptr);
                        ++dptr;
                        ++sptr;
                        --ix;
                    }
                    dptr += dinc;
                    sptr += sinc;
                    --iy;
                }
            }

            //-----------------------------------------------------------------
            template<typename dstT, typename srcT, typename renderT>
            __attribute__((__noinline__))
            void TexturedRectangle(
                dstT*        dstPixels,
                int          dstPitch,
                core::Recti  dstClipRect,
                core::Recti  dstRect,
                srcT*        srcPixels,
                int          srcPitch,
                core::Recti  srcRect,
                renderT      renderer)
            {
                if (dstClipRect.isEmpty() || dstRect.isEmpty() || srcRect.isEmpty()) {
                    return;
                }

                if (dstRect.getDimensions() == srcRect.getDimensions()) {
                    // fall back on simpler algorithm
                    TexturedRectangle(dstPixels, dstPitch, dstClipRect, dstRect.getPosition(), srcPixels, srcPitch, srcRect, renderer);
                    return;
                }

                core::Recti drct = dstRect.getIntersection(dstClipRect);
                if (drct.isEmpty()) {
                    return;
                }

                core::Recti srct(
                    srcRect.getX() + (int)( srcRect.getWidth()  * ( ( drct.getX() - dstRect.getX() ) / (float)dstRect.getWidth()  ) ),
                    srcRect.getY() + (int)( srcRect.getHeight() * ( ( drct.getY() - dstRect.getY() ) / (float)dstRect.getHeight() ) ),
                                     (int)( srcRect.getWidth()  * (   drct.getWidth()                / (float)dstRect.getWidth()  ) ),
                                     (int)( srcRect.getHeight() * (   drct.getHeight()               / (float)dstRect.getHeight() ) )
                );

                const int sy_ptr_inc = (srct.getHeight() / drct.getHeight()) * srcPitch;
                const int sy_nom_inc =  srct.getHeight() % drct.getHeight();
                const int sy_den     =  drct.getHeight();
                const int sx_ptr_inc =  srct.getWidth()  / drct.getWidth();
                const int sx_nom_inc =  srct.getWidth()  % drct.getWidth();
                const int sx_den     =  drct.getWidth();

                const srcT* sy_ptr = srcPixels + (srct.getY() * srcPitch) + srct.getX();
                int         sy_nom = 0;

                const int dinc = dstPitch - drct.getWidth();
                dstT*     dptr = dstPixels + (drct.getY() * dstPitch) + drct.getX();

                int iy = drct.getHeight();
                while (iy > 0) {
                    const srcT* sx_ptr = sy_ptr;
                    int         sx_nom = 0;

                    int ix = drct.getWidth();
                    while (ix > 0) {
                        renderer(dptr, sx_ptr);
                        sx_ptr += sx_ptr_inc;
                        sx_nom += sx_nom_inc;
                        if (sx_nom >= sx_den) {
                            sx_nom -= sx_den;
                            ++sx_ptr;
                        }
                        ++dptr;
                        --ix;
                    }
                    sy_ptr += sy_ptr_inc;
                    sy_nom += sy_nom_inc;
                    if (sy_nom >= sy_den) {
                        sy_nom -= sy_den;
                        sy_ptr += srcPitch;
                    }
                    dptr += dinc;
                    --iy;
                }
            }

            //-----------------------------------------------------------------
            template<typename dstT, typename srcT, typename renderT>
            __attribute__((__noinline__))
            void TexturedQuad(
                dstT*        dstPixels,
                int          dstPitch,
                core::Recti  dstClipRect,
                core::Vec2i  dstPos[4],
                srcT*        srcPixels,
                int          srcPitch,
                core::Recti  srcRect,
                renderT      renderer)
            {
                if (dstClipRect.isEmpty()) {
                    return;
                }

                // find top and bottom points
                int top    = 0;
                int bottom = 0;

                for (int i = 3; i > 0; --i) {
                    if (dstPos[i].y < dstPos[top].y) {
                        top = i;
                    }
                    if (dstPos[i].y > dstPos[bottom].y) {
                        bottom = i;
                    }
                }

                // perform clipping in the y axis
                int minY = minmax(dstPos[top].y,    dstClipRect.ul.y, dstClipRect.lr.y);
                int maxY = minmax(dstPos[bottom].y, dstClipRect.ul.y, dstClipRect.lr.y);

                // precalculate line segment information
                struct segment {
                    // y1 < y2, always
                    int x1, x2;
                    int y1, y2;
                    int u1, u2;
                    int v1, v2;
                } segments[4];

                // segment 0 = top
                // segment 1 = right
                // segment 2 = bottom
                // segment 3 = left

                for (int i = 3; i >= 0; --i) {
                    segment* s = segments + i;

                    int p1 = i;
                    int p2 = (i + 1) & 3;  // x & 3 == x % 4

                    s->x1 = dstPos[p1].x;
                    s->y1 = dstPos[p1].y;
                    s->u1 = (i == 1 || i == 2 ? srcRect.getWidth()  - 1 : 0);
                    s->v1 = (i == 2 || i == 3 ? srcRect.getHeight() - 1 : 0);

                    s->x2 = dstPos[p2].x;
                    s->y2 = dstPos[p2].y;
                    s->u2 = (i == 0 || i == 1 ? srcRect.getWidth()  - 1 : 0);
                    s->v2 = (i == 1 || i == 2 ? srcRect.getHeight() - 1 : 0);

                    if (dstPos[p1].y > dstPos[p2].y) {
                        std::swap(s->x1, s->x2);
                        std::swap(s->y1, s->y2);
                        std::swap(s->u1, s->u2);
                        std::swap(s->v1, s->v2);
                    }
                }

                // draw scanlines
                for (int iy = minY; iy <= maxY; iy++) {
                    // find minimum and maximum x values
                    // initial boundary values
                    int minX = dstClipRect.lr.x + 1;
                    int maxX = dstClipRect.ul.x - 1;

                    // default values in case no
                    int minU = 0;
                    int minV = 0;
                    int maxU = 0;
                    int maxV = 0;

                    // intersect iy in each line
                    for (int i = 3; i >= 0; --i) {
                        segment* s = segments + i;

                        // if iy is even in the segment and segment's length is not zero
                        if (s->y1 <= iy && iy <= s->y2) {
                            int x = (s->y1 == s->y2 ?
                                     s->x1 :
                                     s->x1 + (iy - s->y1) * (s->x2 - s->x1) / (s->y2 - s->y1));
                            int u = (s->y1 == s->y2 ?
                                     s->u1 :
                                     s->u1 + (iy - s->y1) * (s->u2 - s->u1) / (s->y2 - s->y1));
                            int v = (s->y1 == s->y2 ?
                                     s->v1 :
                                     s->v1 + (iy - s->y1) * (s->v2 - s->v1) / (s->y2 - s->y1));

                            // update minimum and maximum x values
                            if (x < minX) {
                                minX = x;
                                minU = u;
                                minV = v;
                            }

                            if (x > maxX) {
                                maxX = x;
                                maxU = u;
                                maxV = v;
                            }
                        }
                    }

                    // now clip the x extents
                    int oldMinX = minX;
                    int oldMaxX = maxX;

                    minX = minmax(minX, dstClipRect.ul.x, dstClipRect.lr.x);
                    maxX = minmax(maxX, dstClipRect.ul.x, dstClipRect.lr.x);

                    // render the scanline pixels
                    if (minX != maxX) {
                        for (int ix = minX; ix <= maxX; ix++) {
                            int iu = minU + (ix - oldMinX) * (maxU - minU) / (oldMaxX - oldMinX);
                            int iv = minV + (ix - oldMinX) * (maxV - minV) / (oldMaxX - oldMinX);

                            renderer(dstPixels + iy * dstPitch + ix, srcPixels + iv * srcPitch + iu);
                        }
                    }
                }
            }

        } // namespace primitives
    } // namespace graphics
} // namespace rpgss


#endif // RPGSS_GRAPHICS_PRIMITIVES_HPP_INCLUDED
