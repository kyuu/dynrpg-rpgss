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

#include <ctime>

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

#include "timer.hpp"


namespace rpgss {
    namespace timer {

        //-----------------------------------------------------------------
        u32 GetTicks()
        {
            return (u32)GetTickCount();
        }

        //-----------------------------------------------------------------
        u32 GetTime()
        {
            return (u32)std::time(0);
        }

        //-----------------------------------------------------------------
        TimeInfo GetTimeInfo(u32 rawtime, bool aslocal)
        {
            time_t _rawtime = (time_t)rawtime;
            tm*    _ti;

            if (aslocal) {
                _ti = std::localtime(&_rawtime);
            } else {
                _ti = std::gmtime(&_rawtime);
            }

            TimeInfo ti;
            ti.second  = _ti->tm_sec;
            ti.minute  = _ti->tm_min;
            ti.hour    = _ti->tm_hour;
            ti.day     = _ti->tm_mday;
            ti.month   = _ti->tm_mon + 1;
            ti.year    = _ti->tm_year + 1900;
            ti.weekday = (_ti->tm_wday == 0 ? 7 : _ti->tm_wday);
            ti.yearday = _ti->tm_yday + 1;

            return ti;
        }

    } // namespace timer
} // namespace rpgss
