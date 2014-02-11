#include "cpuinfo.hpp"


namespace rpgss {
    namespace graphics {

        namespace {

            //-----------------------------------------------------------------
            bool Initialized = false;

            bool MMXSupported = false;
            bool SSESupported = false;
            bool SSE2Supported = false;
            bool SSE3Supported = false;

            //-----------------------------------------------------------------
            void Initialize()
            {
                MMXSupported  = __builtin_cpu_supports("mmx")  != 0;
                SSESupported  = __builtin_cpu_supports("sse")  != 0;
                SSE2Supported = __builtin_cpu_supports("sse2") != 0;
                SSE3Supported = __builtin_cpu_supports("sse3") != 0;

                Initialized = true;
            }

        } // anonymous namespace

        //-----------------------------------------------------------------
        bool CPUSupportsMMX()
        {
            if (!Initialized) {
                Initialize();
            }
            return MMXSupported;
        }

        //-----------------------------------------------------------------
        bool CPUSupportsSSE()
        {
            if (!Initialized) {
                Initialize();
            }
            return SSESupported;
        }

        //-----------------------------------------------------------------
        bool CPUSupportsSSE2()
        {
            if (!Initialized) {
                Initialize();
            }
            return SSE2Supported;
        }

        //-----------------------------------------------------------------
        bool CPUSupportsSSE3()
        {
            if (!Initialized) {
                Initialize();
            }
            return SSE3Supported;
        }

    } // namespace graphics
} // namespace rpgss
