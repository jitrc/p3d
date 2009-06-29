#pragma once

#if defined(P3D_WINDOWS)
#include "Windows/Atomic.h"
#endif

namespace P3D
{
    template<class T>
    class Fenced
    {
    public:
        Fenced()
        { }
        Fenced(const T& t) : _t(t)
        { }
        Fenced& operator=(const T& t)
        {
            _t = t;
            return *this;
        }
        operator T() const
        {
            WriteMemoryBarrier();
            return _t;
        }

        T& Unfenced() { return _t; }
        const T& Unfenced() const { return _t; }

    private:
        T _t;
    };
}