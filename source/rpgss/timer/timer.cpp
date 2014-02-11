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
