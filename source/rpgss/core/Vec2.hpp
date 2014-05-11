#ifndef RPGSS_CORE_VEC2_HPP_INCLUDED
#define RPGSS_CORE_VEC2_HPP_INCLUDED

#include <cmath>

#include "../common/types.hpp"


namespace rpgss {
    namespace core {

        template<typename T>
        struct Vec2 {
            T x;
            T y;

            Vec2() : x(0), y(0) { }

            template<typename U>
            Vec2(U x_, U y_) : x(x_), y(y_) { }

            template<typename U>
            Vec2(const U vec[2]) : x(vec[0]), y(vec[1]) { }

            Vec2(const Vec2& that) : x(that.x), y(that.y) { }

            Vec2& operator=(const Vec2& that) {
                x = that.x;
                y = that.y;
                return *this;
            }

            bool operator==(const Vec2& rhs) const {
                return (x == rhs.x && y == rhs.y);
            }

            bool operator!=(const Vec2& rhs) const {
                return !(*this == rhs);
            }

            Vec2 operator+(const Vec2& rhs) {
                return Vec2(x + rhs.x, y + rhs.y);
            }

            Vec2 operator-(const Vec2& rhs) {
                return Vec2(x - rhs.x, y - rhs.y);
            }

            template<typename U>
            Vec2 operator*(U scalar) {
                return Vec2(x * scalar, y * scalar);
            }

            Vec2& operator+=(const Vec2& rhs) {
                x += rhs.x;
                y += rhs.y;
                return *this;
            }

            Vec2& operator-=(const Vec2& rhs) {
                x -= rhs.x;
                y -= rhs.y;
                return *this;
            }

            template<typename U>
            Vec2& operator*=(U scalar) {
                x = (T)(x * scalar);
                y = (T)(y * scalar);
                return *this;
            }

            float len() const {
                return std::sqrt((float)(x * x + y * y));
            }

            T lenSQ() const {
                return x * x + y * y;
            }

            T dot(const Vec2& rhs) const {
                return x * rhs.x + y * rhs.y;
            }

            Vec2& null() {
                x = 0;
                y = 0;
                return *this;
            }

            Vec2& unit() {
                float l = len();
                x = (T)(x / l);
                y = (T)(y / l);
                return *this;
            }

            float distFrom(const Vec2& that) const {
                return Vec2(x - that.x, y - that.y).len();
            }

            T distFromSQ(const Vec2& that) const {
                return Vec2(x - that.x, y - that.y).lenSQ();
            }

            Vec2& rotateBy(float degrees, const Vec2& center = Vec2()) {
                degrees *= 3.14159f / 180.0f;

                float cs = std::cos(degrees);
                float sn = std::sin(degrees);

                T tx = x - center.x;
                T ty = y - center.y;

                // this code respects that our y-axis goes down
                // to make it work the other way (y-axis goes up),
                // simply swap the signs inside the parentheses
                x = (T)(tx * cs + ty * sn) + center.x;
                y = (T)(ty * cs - tx * sn) + center.y;

                return *this;
            }

            float getAngle() const {
                if (y == 0) {
                    return x < 0.0f ? 180.0f : 0.0f;
                } else if (x == 0.0f) {
                    return y < 0.0f ? 270.0f : 90.0f;
                }
                if ( y > 0.0f) {
                    if (x > 0.0f) {
                        return (float)(std::atan((float)(y / x)) * (180.0f / 3.14159f));
                    } else {
                        return (float)(180.0f - std::atan((float)(y / -x)) * (180.0f / 3.14159f));
                    }
                } else {
                    if (x > 0.0f) {
                        return (float)(360.0f - std::atan((float)(-y / x)) * (180.0f / 3.14159f));
                    } else {
                        return (float)(180.0f + std::atan((float)(-y / -x)) * (180.0f / 3.14159f));
                    }
                }
            }

            float getAngleWith(const Vec2& that) const {
                float tmp = (float)(x*that.x + y*that.y);
                if (tmp == 0.0f) {
                    return 90.0f;
                }
                tmp = tmp / std::sqrt((float)((x*x + y*y) * (that.x*that.x + that.y*that.y)));
                if (tmp < 0.0f) {
                    tmp = -tmp;
                }
                return (float)(std::atan(std::sqrt(1 - tmp*tmp) / tmp) * (180.0f / 3.14159f));
            }

            bool isBetween(const Vec2& begin, const Vec2& end) const {
                if (begin.x != end.x) {
                    return ((begin.x <= x && x <= end.x) ||
                        (begin.x >= x && x >= end.x));
                } else {
                    return ((begin.y <= y && y <= end.y) ||
                        (begin.y >= y && y >= end.y));
                }
            }

            Vec2 getInterpolated(const Vec2& that, float d) const {
                float inv = 1.0f - d;
                return Vec2((T)(that.x*inv + x*d), (T)(that.y*inv + y*d));
            }

            Vec2& interpolate(const Vec2& a, const Vec2& b, float d)
            {
                x = (T)((float)b.x + ( ( a.x - b.x ) * d ));
                y = (T)((float)b.y + ( ( a.y - b.y ) * d ));
                return *this;
            }

        };

        typedef Vec2<i32> Vec2i;
        typedef Vec2<f32> Vec2f;

    } // namespace core
} // namespace rpgss


#endif // RPGSS_CORE_VEC2_HPP_INCLUDED
