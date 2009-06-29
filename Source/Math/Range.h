#pragma once

namespace P3D
{
    /*
    Range [a, b].
    */
    template<typename T = Scalar> 
    class Range
    {
    public:
        Range(const T& _a, const T& _b)
            : a(_a), b(_b)
        { }

    public:
        T a;
        T b;
    };
}