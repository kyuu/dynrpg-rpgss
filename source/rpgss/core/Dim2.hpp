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

#ifndef RPGSS_CORE_DIM2_HPP_INCLUDED
#define RPGSS_CORE_DIM2_HPP_INCLUDED

#include "../common/types.hpp"


namespace rpgss {
    namespace core {

        template<typename T>
        struct Dim2 {
            T width;
            T height;

            Dim2() : width(0), height(0) { }

            template<typename U>
            Dim2(U w, U h) : width(w), height(h) { }

            Dim2(const Dim2& that) : width(that.width), height(that.height) { }

            Dim2& operator=(const Dim2& that) {
                width  = that.width;
                height = that.height;
                return *this;
            }

            bool operator==(const Dim2& that) {
                return (width == that.width && height == that.height);
            }

            bool operator!=(const Dim2& that) {
                return !(*this == that);
            }

            T getArea() const {
                return width * height;
            }

            bool isEmpty() const {
                return width <= 0 || height <= 0;
            }

            Dim2& scale(float s) {
                width  = (T)(width  * s);
                height = (T)(height * s);
                return *this;
            }

            Dim2& scale(float sx, float sy) {
                width  = (T)(width  * sx);
                height = (T)(height * sy);
                return *this;
            }

        };

        typedef Dim2<i32> Dim2i;
        typedef Dim2<f32> Dim2f;

    } // namespace core
} // namespace rpgss


#endif // RPGSS_CORE_DIM2_HPP_INCLUDED
