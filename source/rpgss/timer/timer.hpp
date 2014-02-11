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
