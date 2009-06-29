#pragma once

#ifndef P3D_WINDOWS
#error The file should be included under Windows only.
#endif

#include <intrin.h>

namespace P3D
{
    /*
    Atomically set's *var = newValue and return previous value of var.
    */
    inline long AtomicExchange(long volatile* var, long newValue)
    {
        return InterlockedExchange(var, newValue);
    }

    /*
    Atomcally do this:
        old = *var
        if (*var == compare)
            *var = swap
        return old
    */
    inline long AtomicCAS(long volatile* var, long swap, long compare)
    {
        return InterlockedCompareExchange(var, swap, compare);
    }

    inline int64 AtomicCAS64(int64 volatile* var, int64 swap, int64 compare)
    {
        return InterlockedCompareExchange64(var, swap, compare);
    }


    /*
    Atomically increments variable and return its new value.
    Variable must be aligned to 32 byte.
    */
    inline long AtomicIncrement(long volatile* var)
    {
        return InterlockedIncrement(var);
    }

    /*
    Atomically decrement variable and return its new value.
    Variable must be aligned to 32 byte.
    */
    inline long AtomicDecrement(long volatile* var)
    {
        return InterlockedDecrement(var);
    }

    inline void MemoryFence() { _ReadWriteBarrier(); }
    inline void ReadMemoryBarrier() { _ReadBarrier(); }
    inline void WriteMemoryBarrier() { _WriteBarrier(); }
}