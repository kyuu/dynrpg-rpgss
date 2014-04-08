#ifndef RPGSS_CORE_RECT_HPP_INCLUDED
#define RPGSS_CORE_RECT_HPP_INCLUDED

#include "../common/types.hpp"
#include "Vec2.hpp"
#include "Dim2.hpp"


namespace rpgss {
    namespace core {

        template<typename T>
        struct Rect {
            Vec2<T> ul;
            Vec2<T> lr;

            Rect() : ul(0, 0), lr(-1, -1) { }

            template<typename U>
            Rect(U width, U height) : ul(0, 0), lr(width - 1, height - 1) { }

            Rect(const Dim2<T>& size) : ul(0, 0), lr(size.width - 1, size.height - 1) { }

            template<typename U>
            Rect(U x, U y, U width, U height) : ul(x, y), lr(x + width - 1, y + height - 1) { }

            Rect(const Vec2<T>& pos, const Dim2<T>& size) : ul(pos), lr(pos.x + size.width - 1, pos.y + size.height - 1) { }

            Rect(const Rect& that) : ul(that.ul), lr(that.lr) { }

            Rect& operator=(const Rect& that) {
                ul = that.ul;
                lr = that.lr;
                return *this;
            }

            bool operator==(const Rect& that) {
                return (ul == that.ul &&
                        lr == that.lr);
            }

            bool operator!=(const Rect& that) {
                return !(*this == that);
            }

            T getX() const {
                return ul.x;
            }

            T getY() const {
                return ul.y;
            }

            T getWidth() const {
                return (lr.x - ul.x) + 1;
            }

            T getHeight() const {
                return (lr.y - ul.y) + 1;
            }

            Vec2<T> getUpperLeft() const {
                return Vec2<T>(ul.x, ul.y);
            }

            Vec2<T> getUpperRight() const {
                return Vec2<T>(lr.x, ul.y);
            }

            Vec2<T> getLowerLeft() const {
                return Vec2<T>(ul.x, lr.y);
            }

            Vec2<T> getLowerRight() const {
                return Vec2<T>(lr.x, lr.y);
            }

            Vec2<T> getPosition() const {
                return Vec2<T>(ul.x, ul.y);
            }

            Dim2<T> getDimensions() const {
                return Dim2<T>(getWidth(), getHeight());
            }

            void setX(T x) {
                lr.x = x + (lr.x - ul.x);
                ul.x = x;
            }

            void setY(T y) {
                lr.y = y + (lr.y - ul.y);
                ul.y = y;
            }

            void setWidth(T width) {
                lr.x = ul.x + width - 1;
            }

            void setHeight(T height) {
                lr.y = ul.y + height - 1;
            }

            void setPosition(T x, T y) {
                setX(x);
                setY(y);
            }

            void setPosition(const Vec2<T>& pos) {
                setPosition(pos.x, pos.y);
            }

            void setDimensions(T width, T height) {
                setWidth(width);
                setHeight(height);
            }

            void setDimensions(const Dim2<T>& size) {
                setWidth(size.width);
                setHeight(size.height);
            }

            void setBounds(T x, T y, T width, T height) {
                ul.x = x;
                ul.y = y;
                lr.x = x + width  - 1;
                lr.y = y + height - 1;
            }

            void setBounds(const Vec2<T>& pos, const Dim2<T>& size) {
                ul.x = pos.x;
                ul.y = pos.y;
                lr.x = pos.x + size.width  - 1;
                lr.y = pos.y + size.height - 1;
            }

            Rect& scale(float s) {
                setWidth( (T)(getWidth()  * s));
                setHeight((T)(getHeight() * s));
                return *this;
            }

            Rect& scale(float sx, float sy) {
                setWidth( (T)(getWidth()  * sx));
                setHeight((T)(getHeight() * sy));
                return *this;
            }

            Rect& translate(T ox, T oy) {
                setX(getX() + ox);
                setY(getY() + oy);
                return *this;
            }

            Rect& translate(const Vec2<T>& v) {
                setX(getX() + v.x);
                setY(getY() + v.y);
                return *this;
            }

            T getArea() const {
                return getWidth() * getHeight();
            }

            Vec2<T> getCenter() const {
                return Vec2<T>(
                    getX() + (T)(getWidth()  * 0.5),
                    getY() + (T)(getHeight() * 0.5)
                );
            }

            bool isValid() const {
                return ul.x <= lr.x && ul.y <= lr.y;
            }

            bool isEmpty() const {
                return ul.x > lr.x || ul.y > lr.y;
            }

            bool intersects(const Rect& that) const
            {
                return (lr.y > that.ul.y &&
                        ul.y < that.lr.y &&
                        lr.x > that.ul.x &&
                        ul.x < that.lr.x);
            }

            Rect getIntersection(const Rect& that) const {
                if (ul.x > that.lr.x ||
                    ul.y > that.lr.y ||
                    lr.x < that.ul.x ||
                    lr.y < that.ul.y)
                {
                    return Rect();
                }

                T x1 = ul.x;
                T y1 = ul.y;
                T x2 = lr.x;
                T y2 = lr.y;

                if (x1 < that.ul.x) {
                    x1 = that.ul.x;
                }
                if (x2 > that.lr.x) {
                    x2 = that.lr.x;
                }
                if (y1 < that.ul.y) {
                    y1 = that.ul.y;
                }
                if (y2 > that.lr.y) {
                    y2 = that.lr.y;
                }

                return Rect(x1, y1, (x2 - x1) + 1, (y2 - y1) + 1);
            }

            bool contains(T x, T y) const {
                return (ul.x <= x &&
                        ul.y <= y &&
                        lr.x >= x &&
                        lr.y >= y);
            }

            bool contains(const Vec2<T>& point) const {
                return contains(point.x, point.y);
            }

            bool contains(T x, T y, T width, T height) const {
                return ((x              >= ul.x && x              <= lr.x) &&
                        (x + width  - 1 >= ul.x && x + width  - 1 <= lr.x) &&
                        (y              >= ul.y && y              <= lr.y) &&
                        (y + height - 1 >= ul.y && y + height - 1 <= lr.y));
            }

            bool contains(const Rect<T>& rect) const {
                return ((rect.ul.x >= ul.x && rect.ul.x <= lr.x) &&
                        (rect.lr.x >= ul.x && rect.lr.x <= lr.x) &&
                        (rect.ul.y >= ul.y && rect.ul.y <= lr.y) &&
                        (rect.lr.y >= ul.y && rect.lr.y <= lr.y));
            }

            bool isInside(T x, T y, T width, T height) const {
                return ((ul.x >= x && ul.x <= x + width  - 1) &&
                        (lr.x >= x && lr.x <= x + width  - 1) &&
                        (ul.y >= y && ul.y <= y + height - 1) &&
                        (lr.y >= y && lr.y <= y + height - 1));
            }

            bool isInside(const Rect<T>& rect) const {
                return ((ul.x >= rect.ul.x && ul.x <= rect.lr.x) &&
                        (lr.x >= rect.ul.x && lr.x <= rect.lr.x) &&
                        (ul.y >= rect.ul.y && ul.y <= rect.lr.y) &&
                        (lr.y >= rect.ul.y && lr.y <= rect.lr.y));
            }
        };

        typedef Rect<i32> Recti;
        typedef Rect<f32> Rectf;

    } // namespace core
} // namespace rpgss


#endif // RPGSS_CORE_RECT_HPP_INCLUDED
