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

#ifndef RPGSS_TIMER_TIMER_HPP_INCLUDED
#define RPGSS_TIMER_TIMER_HPP_INCLUDED

#include "../common/types.hpp"


namespace rpgss {
    namespace timer {

        struct TimeInfo {
            u32 second;  // 0-59
            u32 minute;  // 0-59
            u32 hour;    // 0-23
            u32 day;     // 1-31
            u32 month;   // 1-12
            u32 year;
            u32 weekday; // 1-7
            u32 yearday; // 1-366
        };

        u32 GetTicks();
        u32 GetTime();
        TimeInfo GetTimeInfo(u32 rawtime, bool aslocal = true);

    } // namespace timer
} // namespace rpgss


#endif // RPGSS_TIMER_TIMER_HPP_INCLUDED
