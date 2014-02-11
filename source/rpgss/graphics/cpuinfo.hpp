#ifndef RPGSS_GRAPHICS_CPUINFO_HPP_INCLUDED
#define RPGSS_GRAPHICS_CPUINFO_HPP_INCLUDED


namespace rpgss {
    namespace graphics {

        bool CPUSupportsMMX();
        bool CPUSupportsSSE();
        bool CPUSupportsSSE2();
        bool CPUSupportsSSE3();

    } // namespace graphics
} // namespace rpgss


#endif // RPGSS_GRAPHICS_GRAPHICS_HPP_INCLUDED
