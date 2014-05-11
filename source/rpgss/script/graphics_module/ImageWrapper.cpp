#include <cassert>
#include <cstring>

#include "../../Context.hpp"
#include "../core_module/core_module.hpp"
#include "constants.hpp"
#include "FontWrapper.hpp"
#include "WindowSkinWrapper.hpp"
#include "ImageWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace graphics_module {

            //---------------------------------------------------------
            void
            ImageWrapper::Push(lua_State* L, graphics::Image* image)
            {
                assert(image);
                luabridge::push(L, ImageWrapper(image));
            }

            //---------------------------------------------------------
            bool
            ImageWrapper::Is(lua_State* L, int index)
            {
                assert(index != 0);

                if (index < 0) { // allow negative indices
                    index = lua_gettop(L) + index + 1;
                }

                return luabridge::Stack<ImageWrapper*>::is_a(L, index);
            }

            //---------------------------------------------------------
            graphics::Image*
            ImageWrapper::Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "Image expected, got nothing");
                    return 0;
                }
                ImageWrapper* wrapper = luabridge::Stack<ImageWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "Image expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            graphics::Image*
            ImageWrapper::GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                ImageWrapper* wrapper = luabridge::Stack<ImageWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            ImageWrapper::ImageWrapper(graphics::Image* ptr)
                : This(ptr)
            {
            }

            //---------------------------------------------------------
            int
            ImageWrapper::get_width() const
            {
                return This->getWidth();
            }

            //---------------------------------------------------------
            int
            ImageWrapper::get_height() const
            {
                return This->getHeight();
            }

            //---------------------------------------------------------
            int
            ImageWrapper::__len(lua_State* L)
            {
                lua_pushnumber(L, This->getSizeInPixels());
                return 1;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::getDimensions(lua_State* L)
            {
                lua_pushnumber(L, This->getWidth());
                lua_pushnumber(L, This->getHeight());
                return 2;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::getBlendMode(lua_State* L)
            {
                std::string blend_mode_str;
                if (!GetBlendModeConstant(This->getBlendMode(), blend_mode_str)) {
                    return luaL_error(L, "unexpected internal value");
                }
                lua_pushstring(L, blend_mode_str.c_str());
                return 1;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::setBlendMode(lua_State* L)
            {
                const char* blend_mode_str = luaL_checkstring(L, 2);
                int blend_mode;
                if (!GetBlendModeConstant(blend_mode_str, blend_mode)) {
                    return luaL_argerror(L, 2, "invalid blend mode constant");
                }
                This->setBlendMode(blend_mode);
                return 0;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::getClipRect(lua_State* L)
            {
                core::Recti clip_rect = This->getClipRect();
                lua_pushnumber(L, clip_rect.getX());
                lua_pushnumber(L, clip_rect.getY());
                lua_pushnumber(L, clip_rect.getWidth());
                lua_pushnumber(L, clip_rect.getHeight());
                return 4;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::setClipRect(lua_State* L)
            {
                int x = luaL_checkint(L, 2);
                int y = luaL_checkint(L, 3);
                int w = luaL_checkint(L, 4);
                int h = luaL_checkint(L, 5);

                core::Recti clip_rect = core::Recti(x, y, w, h);
                core::Recti image_bounds = core::Recti(This->getDimensions());

                if (!clip_rect.isValid() || !clip_rect.isInside(image_bounds)) {
                    return luaL_error(L, "invalid rect");
                }

                This->setClipRect(clip_rect);
                return 0;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::copyPixels(lua_State* L)
            {
                core::ByteArray::Ptr pixels = core::ByteArray::New((const u8*)This->getPixels(), This->getSizeInBytes());
                core_module::ByteArrayWrapper::Push(L, pixels);
                return 1;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::copyRect(lua_State* L)
            {
                int x = luaL_checkint(L, 2);
                int y = luaL_checkint(L, 3);
                int w = luaL_checkint(L, 4);
                int h = luaL_checkint(L, 5);
                graphics::Image* destination = ImageWrapper::GetOpt(L, 6);

                core::Recti rect = core::Recti(x, y, w, h);
                core::Recti image_bounds = core::Recti(This->getDimensions());

                if (!rect.isValid() || !rect.isInside(image_bounds)) {
                    return luaL_error(L, "invalid rect");
                }

                luabridge::push(L, ImageWrapper(This->copyRect(rect, destination)));
                return 1;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::resize(lua_State* L)
            {
                int width  = luaL_checkint(L, 2);
                int height = luaL_checkint(L, 3);
                luaL_argcheck(L, width  > 0, 2, "invalid width");
                luaL_argcheck(L, height > 0, 3, "invalid height");
                This->resize(width, height);
                return 0;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::setAlpha(lua_State* L)
            {
                int alpha = luaL_checkint(L, 2);
                This->setAlpha(alpha);
                return 0;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::clear(lua_State* L)
            {
                u32 color = luaL_optint(L, 2, 0x000000FF);
                This->clear(graphics::RGBA8888ToRGBA(color));
                return 0;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::grey(lua_State* L)
            {
                This->grey();
                return 0;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::flip(lua_State* L)
            {
                const char* axis_str = luaL_checkstring(L, 2);

                if (std::strcmp(axis_str, "x-axis") == 0) {
                    This->flipVertical();
                } else if (std::strcmp(axis_str, "y-axis") == 0) {
                    This->flipHorizontal();
                } else {
                    return luaL_argerror(L, 2, "invalid axis constant");
                }

                return 0;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::rotate(lua_State* L)
            {
                const char* direction_str = luaL_checkstring(L, 2);

                if (std::strcmp(direction_str, "cw") == 0) {
                    This->rotateClockwise();
                } else if (std::strcmp(direction_str, "ccw") == 0) {
                    This->rotateCounterClockwise();
                } else {
                    return luaL_error(L, "invalid direction constant");
                }

                return 0;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::getPixel(lua_State* L)
            {
                int x = luaL_checkint(L, 2);
                int y = luaL_checkint(L, 3);

                luaL_argcheck(L, x >= 0 && x < This->getWidth(),  2, "invalid x");
                luaL_argcheck(L, y >= 0 && y < This->getHeight(), 3, "invalid y");

                lua_pushinteger(L, graphics::RGBAToRGBA8888(This->getPixel(x, y)));

                return 1;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::setPixel(lua_State* L)
            {
                int x = luaL_checkint(L, 2);
                int y = luaL_checkint(L, 3);
                u32 c = luaL_checkint(L, 4);

                luaL_argcheck(L, x >= 0 && x < This->getWidth(),  2, "invalid x");
                luaL_argcheck(L, y >= 0 && y < This->getHeight(), 3, "invalid y");

                This->setPixel(x, y, graphics::RGBA8888ToRGBA(c));

                return 0;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::drawPoint(lua_State* L)
            {
                int x = luaL_checkint(L, 2);
                int y = luaL_checkint(L, 3);
                u32 c = luaL_checkint(L, 4);

                This->drawPoint(
                    core::Vec2i(x, y),
                    graphics::RGBA8888ToRGBA(c)
                );

                return 0;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::drawLine(lua_State* L)
            {
                int x1 = luaL_checkint(L, 2);
                int y1 = luaL_checkint(L, 3);
                int x2 = luaL_checkint(L, 4);
                int y2 = luaL_checkint(L, 5);
                u32 c1 = luaL_checkint(L, 6);
                u32 c2 = luaL_optint(L, 7, c1);

                This->drawLine(
                    core::Vec2i(x1, y1),
                    core::Vec2i(x2, y2),
                    graphics::RGBA8888ToRGBA(c1),
                    graphics::RGBA8888ToRGBA(c2)
                );

                return 0;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::drawRectangle(lua_State* L)
            {
                bool fill = lua_toboolean(L, 2);
                int     x = luaL_checkint(L, 3);
                int     y = luaL_checkint(L, 4);
                int     w = luaL_checkint(L, 5);
                int     h = luaL_checkint(L, 6);
                u32    c1 = luaL_checkint(L, 7);
                u32    c2 = luaL_optint(L, 8, c1);
                u32    c3 = luaL_optint(L, 9, c1);
                u32    c4 = luaL_optint(L, 10, c1);

                This->drawRectangle(
                    fill,
                    core::Recti(x, y, w, h),
                    graphics::RGBA8888ToRGBA(c1),
                    graphics::RGBA8888ToRGBA(c2),
                    graphics::RGBA8888ToRGBA(c3),
                    graphics::RGBA8888ToRGBA(c4)
                );

                return 0;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::drawCircle(lua_State* L)
            {
                bool fill = lua_toboolean(L, 2);
                int     x = luaL_checkint(L, 3);
                int     y = luaL_checkint(L, 4);
                int     r = luaL_checkint(L, 5);
                u32    c1 = luaL_checkint(L, 6);
                u32    c2 = luaL_optint(L, 7, c1);

                This->drawCircle(
                    fill,
                    core::Vec2i(x, y),
                    r,
                    graphics::RGBA8888ToRGBA(c1),
                    graphics::RGBA8888ToRGBA(c2)
                );

                return 0;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::drawTriangle(lua_State* L)
            {
                /*
                bool fill = lua_toboolean(L, 2);
                int    x1 = luaL_checkint(L, 3);
                int    y1 = luaL_checkint(L, 4);
                int    x2 = luaL_checkint(L, 5);
                int    y2 = luaL_checkint(L, 6);
                int    x3 = luaL_checkint(L, 7);
                int    y3 = luaL_checkint(L, 8);
                u32    c1 = luaL_checkunsigned(L, 9);
                u32    c2 = luaL_optunsigned(L, 10, c1);
                u32    c3 = luaL_optunsigned(L, 11, c1);
                */

                // TODO

                return 0;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::draw(lua_State* L)
            {
                graphics::Image* that = 0;
                int sx, sy, sw, sh;
                int x, y;
                float rotate;
                float scale;
                u32   color;

                int nargs = lua_gettop(L);
                if (nargs >= 8)
                {
                    that = ImageWrapper::Get(L, 2);
                    sx   = luaL_checkint(L, 3);
                    sy   = luaL_checkint(L, 4);
                    sw   = luaL_checkint(L, 5);
                    sh   = luaL_checkint(L, 6);
                    x    = luaL_checkint(L, 7);
                    y    = luaL_checkint(L, 8);
                    rotate = luaL_optnumber(L, 9, 0.0);
                    scale  = luaL_optnumber(L, 10, 1.0);
                    color  = luaL_optint(L, 11, 0xFFFFFFFF);
                }
                else
                {
                    that = ImageWrapper::Get(L, 2);
                    x    = luaL_checkint(L, 3);
                    y    = luaL_checkint(L, 4);
                    rotate = luaL_optnumber(L, 5, 0.0);
                    scale  = luaL_optnumber(L, 6, 1.0);
                    color  = luaL_optint(L, 7, 0xFFFFFFFF);

                    sx = 0;
                    sy = 0;
                    sw = that->getWidth();
                    sh = that->getHeight();
                }

                This->draw(
                    that,
                    core::Recti(sx, sy, sw, sh),
                    core::Vec2i(x, y),
                    rotate,
                    scale,
                    graphics::RGBA8888ToRGBA(color)
                );

                return 0;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::drawq(lua_State* L)
            {
                graphics::Image* that = 0;
                int sx, sy, sw, sh;
                int x1, y1;
                int x2, y2;
                int x3, y3;
                int x4, y4;
                u32 color;

                int nargs = lua_gettop(L);
                if (nargs >= 14)
                {
                    that = ImageWrapper::Get(L, 2);
                    sx   = luaL_checkint(L, 3);
                    sy   = luaL_checkint(L, 4);
                    sw   = luaL_checkint(L, 5);
                    sh   = luaL_checkint(L, 6);
                    x1   = luaL_checkint(L, 7);
                    y1   = luaL_checkint(L, 8);
                    x2   = luaL_checkint(L, 9);
                    y2   = luaL_checkint(L, 10);
                    x3   = luaL_checkint(L, 11);
                    y3   = luaL_checkint(L, 12);
                    x4   = luaL_checkint(L, 13);
                    y4   = luaL_checkint(L, 14);
                    color = luaL_optint(L, 15, 0xFFFFFFFF);
                }
                else
                {
                    that = ImageWrapper::Get(L, 2);
                    x1   = luaL_checkint(L, 3);
                    y1   = luaL_checkint(L, 4);
                    x2   = luaL_checkint(L, 5);
                    y2   = luaL_checkint(L, 6);
                    x3   = luaL_checkint(L, 7);
                    y3   = luaL_checkint(L, 8);
                    x4   = luaL_checkint(L, 9);
                    y4   = luaL_checkint(L, 10);
                    color = luaL_optint(L, 11, 0xFFFFFFFF);

                    sx = 0;
                    sy = 0;
                    sw = that->getWidth();
                    sh = that->getHeight();
                }

                This->drawq(
                    that,
                    core::Recti(sx, sy, sw, sh),
                    core::Vec2i(x1, y1),
                    core::Vec2i(x2, y2),
                    core::Vec2i(x3, y3),
                    core::Vec2i(x4, y4),
                    graphics::RGBA8888ToRGBA(color)
                );

                return 0;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::drawText(lua_State* L)
            {
                graphics::Font* font = FontWrapper::Get(L, 2);
                int x                = luaL_checkint(L, 3);
                int y                = luaL_checkint(L, 4);
                size_t len;
                const char* text     = luaL_checklstring(L, 5, &len);
                float scale          = luaL_optnumber(L, 6, 1.0);
                u32 color            = luaL_optint(L, 7, 0xFFFFFFFF);

                This->drawText(
                    font,
                    core::Vec2i(x, y),
                    text,
                    len,
                    scale,
                    graphics::RGBA8888ToRGBA(color)
                );

                return 0;
            }

            //---------------------------------------------------------
            int
            ImageWrapper::drawWindow(lua_State* L)
            {
                graphics::WindowSkin* windowSkin = WindowSkinWrapper::Get(L, 2);
                int x = luaL_checkint(L, 3);
                int y = luaL_checkint(L, 4);
                int w = luaL_checkint(L, 5);
                int h = luaL_checkint(L, 6);
                int opacity = luaL_optint(L, 7, 255);

                luaL_argcheck(L, w >= 0, 5, "invalid width");
                luaL_argcheck(L, h >= 0, 6, "invalid height");
                luaL_argcheck(L, opacity >= 0 && opacity <= 255, 7, "invalid opacity");

                This->drawWindow(
                    windowSkin,
                    core::Recti(x, y, w, h),
                    opacity
                );

                return 0;
            }

        } // namespace graphics_module
    } // namespace script
} // namespace rpgss
