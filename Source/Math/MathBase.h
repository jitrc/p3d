#pragma once

#if !defined(ALIGNED_VARIABLE)
#    define ALIGNED_VARIABLE(NAME, ALIGNMENT) __declspec(align(ALIGNMENT)) NAME
#endif //ALIGNED_VARIABLE

// Define paranoia to test for uninitialized vectors bug.
// In paranoia mode vector constructor initializes x, y, z with VERY big values...
#define TD_MATH_PARANOIA 

#define mathinline forceinline

namespace P3D
{
    typedef float Scalar;

    const Scalar DEFAULT_EPSILON = 0.001f;

    /*
    Possible results of    Intersects calls.
    */
    enum IntersectionResult
    {
        INSIDE, // some implementations returns INTERSECTS insteed (faster to check...)
        OUTSIDE,
        INTERSECTS
    };

    mathinline Scalar Det2(Scalar m00, Scalar m01,
                         Scalar m10, Scalar m11)
    {
        return m00*m11 - m10*m01;
    }

    mathinline Scalar Det3(Scalar m00, Scalar m01, Scalar m02,
                         Scalar m10, Scalar m11, Scalar m12,
                         Scalar m20, Scalar m21, Scalar m22)
    {
        Scalar out = 0;
        out += m00 * Det2(m11, m12, m21, m22);
        out -= m01 * Det2(m10, m12, m20, m22);
        out += m02 * Det2(m10, m11, m20, m21);
        return out;
    }

    mathinline Scalar Det4(Scalar m00, Scalar m01, Scalar m02, Scalar m03,
                         Scalar m10, Scalar m11, Scalar m12, Scalar m13,
                         Scalar m20, Scalar m21, Scalar m22, Scalar m23,
                         Scalar m30, Scalar m31, Scalar m32, Scalar m33)
    {
        Scalar out = 0;
        out += m00 * Det3(m11, m12, m13, m21, m22, m23, m31, m32, m33);
        out -= m01 * Det3(m10, m12, m13, m20, m22, m23, m30, m32, m33);
        out += m02 * Det3(m10, m11, m13, m20, m21, m23, m30, m31, m33);
        out -= m03 * Det3(m10, m11, m12, m20, m21, m22, m30, m31, m32);
        return out;
    }

    mathinline Scalar sqrt(Scalar r) { return Scalar(::sqrt(r)); }
    mathinline Scalar sqrtInv(Scalar r) { return Scalar(1.0) / Scalar(::sqrt(r)); }

    mathinline Scalar sin(Scalar r) { return Scalar(::sin(r)); }
    mathinline Scalar cos(Scalar r) { return Scalar(::cos(r)); }
    mathinline Scalar abs(Scalar r) { return Scalar(::fabs(r)); }

    mathinline void sincos(float x, float* s, float* c)
    {
        *s = sin(x);
        *c = cos(x);
    }
}

#include "Functions.h"