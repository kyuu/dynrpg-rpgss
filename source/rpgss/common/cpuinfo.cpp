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
