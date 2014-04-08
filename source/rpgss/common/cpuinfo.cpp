#include "cpuinfo.hpp"


namespace rpgss {

    namespace {

        //-----------------------------------------------------------------
        bool MmxSupported = false;
        bool SseSupported = false;
        bool Sse2Supported = false;
        bool Sse3Supported = false;

        //-----------------------------------------------------------------
        bool Initialize()
        {
            MmxSupported  = __builtin_cpu_supports("mmx")  != 0;
            SseSupported  = __builtin_cpu_supports("sse")  != 0;
            Sse2Supported = __builtin_cpu_supports("sse2") != 0;
            Sse3Supported = __builtin_cpu_supports("sse3") != 0;

            return true;
        }
        bool _initialized = Initialize();

    } // anonymous namespace

    //-----------------------------------------------------------------
    bool CpuSupportsMmx()
    {
        return MmxSupported;
    }

    //-----------------------------------------------------------------
    bool CpuSupportsSse()
    {
        return SseSupported;
    }

    //-----------------------------------------------------------------
    bool CpuSupportsSse2()
    {
        return Sse2Supported;
    }

    //-----------------------------------------------------------------
    bool CpuSupportsSse3()
    {
        return Sse3Supported;
    }

} // namespace rpgss
