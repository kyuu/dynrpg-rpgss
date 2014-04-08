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
