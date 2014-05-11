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

#include "../core_module/core_module.hpp"
#include "../io_module/io_module.hpp"
#include "graphics_module.hpp"


namespace rpgss {
    namespace script {
        namespace graphics_module {

            //---------------------------------------------------------
            int graphics_packColor(lua_State* L)
            {
                u8 r = luaL_checkint(L, 1);
                u8 g = luaL_checkint(L, 2);
                u8 b = luaL_checkint(L, 3);
                u8 a = luaL_optint(L, 4, 255);
                lua_pushinteger(L, graphics::RGBA8888(r, g, b, a));
                return 1;
            }

            //---------------------------------------------------------
            int graphics_unpackColor(lua_State* L)
            {
                u32 c = luaL_checkint(L, 1);

                graphics::RGBA unpacked = graphics::RGBA8888ToRGBA(c);
                lua_pushinteger(L, unpacked.red);
                lua_pushinteger(L, unpacked.green);
                lua_pushinteger(L, unpacked.blue);
                lua_pushinteger(L, unpacked.alpha);

                return 4;
            }

            //---------------------------------------------------------
            int graphics_newFont(lua_State* L)
            {
                if (lua_isstring(L, 1))
                {
                    // newFont(filename)
                    const char* filename = luaL_checkstring(L, 1);

                    graphics::Image::Ptr fontImage = graphics::ReadImage(filename);
                    if (fontImage) {
                        graphics::Font::Ptr font = graphics::Font::New(fontImage);
                        if (font) {
                            FontWrapper::Push(L, font);
                        } else {
                            lua_pushnil(L);
                        }
                    } else {
                        lua_pushnil(L);
                    }
                }
                else
                {
                    // newFont(fontImage)
                    graphics::Image* fontImage = ImageWrapper::Get(L, 1);
                    graphics::Font::Ptr font = graphics::Font::New(fontImage);
                    if (font) {
                        FontWrapper::Push(L, font);
                    } else {
                        lua_pushnil(L);
                    }
                }

                return 1;
            }

            //---------------------------------------------------------
            int graphics_newWindowSkin(lua_State* L)
            {
                if (lua_isstring(L, 1))
                {
                    // newWindowSkin(filename)
                    const char* filename = luaL_checkstring(L, 1);

                    graphics::Image::Ptr windowSkinImage = graphics::ReadImage(filename);
                    if (windowSkinImage) {
                        graphics::WindowSkin::Ptr windowSkin = graphics::WindowSkin::New(windowSkinImage);
                        if (windowSkin) {
                            WindowSkinWrapper::Push(L, windowSkin);
                        } else {
                            lua_pushnil(L);
                        }
                    } else {
                        lua_pushnil(L);
                    }
                }
                else
                {
                    // newWindowSkin(windowSkinImage)
                    graphics::Image* windowSkinImage = ImageWrapper::Get(L, 1);
                    graphics::WindowSkin::Ptr windowSkin = graphics::WindowSkin::New(windowSkinImage);
                    if (windowSkin) {
                        WindowSkinWrapper::Push(L, windowSkin);
                    } else {
                        lua_pushnil(L);
                    }
                }

                return 1;
            }

            //---------------------------------------------------------
            int graphics_newImage(lua_State* L)
            {
                int nargs = lua_gettop(L);
                if (nargs >= 3 && core_module::ByteArrayWrapper::Is(L, 3))
                {
                    // newImage(width, height, pixels)
                    int w = luaL_checkint(L, 1);
                    int h = luaL_checkint(L, 2);
                    core::ByteArray* pixels = core_module::ByteArrayWrapper::Get(L, 3);

                    luaL_argcheck(L, w > 0, 1, "invalid width");
                    luaL_argcheck(L, h > 0, 2, "invalid height");
                    luaL_argcheck(L, pixels->getSize() == w * h * 4, 3, "invalid pixels");

                    graphics::Image::Ptr image = graphics::Image::New(w, h, (const graphics::RGBA*)pixels->getBuffer());
                    ImageWrapper::Push(L, image);
                }
                else
                {
                    // newImage(width, height [, color])
                    int w = luaL_checkint(L, 1);
                    int h = luaL_checkint(L, 2);
                    u32 c = luaL_optint(L, 3, 0x000000FF);

                    luaL_argcheck(L, w > 0, 1, "invalid width");
                    luaL_argcheck(L, h > 0, 2, "invalid height");

                    graphics::Image::Ptr image = graphics::Image::New(w, h, graphics::RGBA8888ToRGBA(c));
                    ImageWrapper::Push(L, image);
                }
                return 1;
            }

            //---------------------------------------------------------
            int graphics_readImage(lua_State* L)
            {
                if (lua_isstring(L, 1))
                {
                    // readImage(filename)
                    const char* filename = lua_tostring(L, 1);
                    graphics::Image::Ptr image = graphics::ReadImage(filename);
                    if (image) {
                        ImageWrapper::Push(L, image);
                    } else {
                        lua_pushnil(L);
                    }
                }
                else
                {
                    // readImage(stream)
                    io::File* file = io_module::FileWrapper::Get(L, 1);
                    graphics::Image::Ptr image = graphics::ReadImage(file);
                    if (image) {
                        ImageWrapper::Push(L, image);
                    } else {
                        lua_pushnil(L);
                    }
                }
                return 1;
            }

            //---------------------------------------------------------
            int graphics_writeImage(lua_State* L)
            {
                int nargs = lua_gettop(L);
                if (nargs >= 2 && lua_isstring(L, 2))
                {
                    // writeImage(image, filename)
                    graphics::Image* image = ImageWrapper::Get(L, 1);
                    const char* filename = luaL_checkstring(L, 2);
                    lua_pushboolean(L, graphics::WriteImage(image, filename));
                }
                else
                {
                    // writeImage(image, stream)
                    graphics::Image* image = ImageWrapper::Get(L, 1);
                    io::File* file = io_module::FileWrapper::Get(L, 2);
                    lua_pushboolean(L, graphics::WriteImage(image, file));
                }
                return 1;
            }

            //---------------------------------------------------------
            bool RegisterGraphicsModule(lua_State* L)
            {
                luabridge::getGlobalNamespace(L)
                    .beginNamespace("graphics")

                        .addCFunction("packColor",   &graphics_packColor)
                        .addCFunction("unpackColor", &graphics_unpackColor)

                        .beginClass<FontWrapper>("Font")
                            .addProperty("maxCharWidth",        &FontWrapper::get_maxCharWidth)
                            .addProperty("maxCharHeight",       &FontWrapper::get_maxCharHeight)
                            .addProperty("tabWidth",            &FontWrapper::get_tabWidth,          &FontWrapper::set_tabWidth)
                            .addCFunction("getTextWidth",       &FontWrapper::getTextWidth)
                            .addCFunction("wordWrapText",       &FontWrapper::wordWrapText)
                        .endClass()

                        .addCFunction("newFont", &graphics_newFont)

                        .beginClass<WindowSkinWrapper>("WindowSkin")
                            .addProperty("topLeftColor",     &WindowSkinWrapper::get_topLeftColor,     &WindowSkinWrapper::set_topLeftColor)
                            .addProperty("topRightColor",    &WindowSkinWrapper::get_topRightColor,    &WindowSkinWrapper::set_topRightColor)
                            .addProperty("bottomRightColor", &WindowSkinWrapper::get_bottomRightColor, &WindowSkinWrapper::set_bottomRightColor)
                            .addProperty("bottomLeftColor",  &WindowSkinWrapper::get_bottomLeftColor,  &WindowSkinWrapper::set_bottomLeftColor)
                        .endClass()

                        .addCFunction("newWindowSkin", &graphics_newWindowSkin)

                        .beginClass<ImageWrapper>("Image")
                            .addProperty("width",               &ImageWrapper::get_width)
                            .addProperty("height",              &ImageWrapper::get_height)
                            .addCFunction("__len",              &ImageWrapper::__len)
                            .addCFunction("getDimensions",      &ImageWrapper::getDimensions)
                            .addCFunction("getClipRect",        &ImageWrapper::getClipRect)
                            .addCFunction("setClipRect",        &ImageWrapper::setClipRect)
                            .addCFunction("copyPixels",         &ImageWrapper::copyPixels)
                            .addCFunction("copyRect",           &ImageWrapper::copyRect)
                            .addCFunction("resize",             &ImageWrapper::resize)
                            .addCFunction("setAlpha",           &ImageWrapper::setAlpha)
                            .addCFunction("clear",              &ImageWrapper::clear)
                            .addCFunction("grey",               &ImageWrapper::grey)
                            .addCFunction("flip",               &ImageWrapper::flip)
                            .addCFunction("rotate",             &ImageWrapper::rotate)
                            .addCFunction("getPixel",           &ImageWrapper::getPixel)
                            .addCFunction("setPixel",           &ImageWrapper::setPixel)
                            .addCFunction("drawPoint",          &ImageWrapper::drawPoint)
                            .addCFunction("drawLine",           &ImageWrapper::drawLine)
                            .addCFunction("drawRectangle",      &ImageWrapper::drawRectangle)
                            .addCFunction("drawCircle",         &ImageWrapper::drawCircle)
                            .addCFunction("drawTriangle",       &ImageWrapper::drawTriangle)
                            .addCFunction("draw",               &ImageWrapper::draw)
                            .addCFunction("drawq",              &ImageWrapper::drawq)
                            .addCFunction("drawText",           &ImageWrapper::drawText)
                            .addCFunction("drawWindow",         &ImageWrapper::drawWindow)
                        .endClass()

                        .addCFunction("newImage",   &graphics_newImage)
                        .addCFunction("readImage",  &graphics_readImage)
                        .addCFunction("writeImage", &graphics_writeImage)

                    .endNamespace();

                return true;
            }

        } // graphics_module
    } // namespace script
} // namespace rpgss
