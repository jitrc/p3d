#include "Includes.h"
#include "TimeCounter.h"

#if !defined(P3D_WINDOWS)
#error TODO: implement for other platforms
#endif

namespace P3D
{
    TimeCounter::TimeCounter()
    {
        QueryPerformanceFrequency((LARGE_INTEGER*)&_ticksPerSec);
        Reset();
    }

    uint64 TimeCounter::GetTickCount()
    {
        uint64 res;
        QueryPerformanceCounter((LARGE_INTEGER*)&res);
        return res;
    }
}