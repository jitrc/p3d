#pragma once

namespace P3D
{
    template<class T>
    T Min(const T m1, const T m2)
    {
        return (m1 < m2) ? m1 : m2;
    }

    template<class T>
    T Max(const T m1, const T m2)
    {
        return (m1 > m2) ? m1 : m2;
    }

    template<class T>
    void Clamp(T& a, const T min, const T max)
    {
        if (a < min) { a = min; return; }
        if (a > max) { a = max; return; }
    }

    /*
    Computes <b>not negative</b> power of the t.
    */
    template<class T>
    mathinline const T Power(const T& t, int N)
    {
        T res = 1;
        // TODO: change to subdivision? ( i = i >> 1 )
        for (int i = 0; i < N; i++) 
            res *= t;
        return res;
    }    
}