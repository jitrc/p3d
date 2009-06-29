#pragma once

#include "Quaternion.h"
#include "Vector.h"

namespace P3D
{
    mathinline Quaternion::Quaternion()
    {
#ifdef TD_MATH_PARANOIA
        x = y = z = w = 1000000.0f;
#endif
    }

    mathinline Quaternion::Quaternion(Scalar a, Scalar b, Scalar c, Scalar d)
        : x(a), y(b), z(c), w(d)
    {
    }

    mathinline Quaternion::Quaternion(const Vector& axis, Scalar angle)
    {
        SetAxisAngle(axis, angle);
    }

    mathinline void Quaternion::Set(Scalar a, Scalar b, Scalar c, Scalar d)
    {
        x = a;
        y = b;
        z = c;
        w = d;
    }

    mathinline void Quaternion::SetIdentity()
    {
        this->Set(0, 0, 0, 1);
    }

    mathinline void Quaternion::SetAxisAngle(const Vector& axis, Scalar angle)
    {
        ASSERT(abs(axis.Length() - 1.0f) < 0.01f); // is axis normalized?

        Scalar sin_half;
        Scalar cos_half;
        sincos(angle*0.5f, &sin_half, &cos_half);
        
        x = axis.x * sin_half;
        y = axis.y * sin_half;
        z = axis.z * sin_half;
        w = cos_half;
    }

    mathinline void Quaternion::SetEulerAngles(Scalar yaw, Scalar pitch, Scalar roll)
    {
        Scalar cr, cp, cy, sr, sp, sy, cpcy, spsy;

        sincos(yaw*0.5f, &sy, &cy);
        sincos(pitch*0.5f, &sp, &cp);
        sincos(roll*0.5f, &sr, &cr);

        cpcy = cp * cy;
        spsy = sp * sy;
    
        x = sr * cpcy - cr * spsy;
        y = cr * sp * cy + sr * cp * sy;
        z = cr * cp * sy - sr * sp * cy;
        w = cr * cpcy + sr * spsy;
    }

    mathinline void Quaternion::Normalize()
    {
        Scalar invLen = sqrtInv(x*x + y*y + z*z + w*w);
        x *= invLen;
        y *= invLen;
        z *= invLen;
        w *= invLen;
    }

    mathinline void Quaternion::SetInterpolation(const Quaternion& q1, const Quaternion& q2, Scalar t)
    {
        // See http://en.wikipedia.org/wiki/Slerp#Quaternion_Slerp

        Scalar p1[4];
        Scalar omega, cosom, sinom, scale0, scale1;

        // cos
        cosom = q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;

        if (cosom < 0.0)
        { 
            cosom = - cosom;
            p1[0] = - q2.x;  p1[1] = - q2.y;
            p1[2] = - q2.z;  p1[3] = - q2.w;
        }
        else
        {
            p1[0] = q2.x;    p1[1] = q2.y;
            p1[2] = q2.z;    p1[3] = q2.w;
        }

        const Scalar DELTA = 0.0001f;

        if ((1.0 - cosom) > DELTA)
        {
            // slerp
            omega = acos(cosom);
            sinom = sin(omega);
            scale0 = sin((Scalar(1.0) - t) * omega) / sinom;
            scale1 = sin(t * omega) / sinom;
        }
        else
        {        
            // linear
            scale0 = Scalar(1.0) - t;
            scale1 = t;
        }

        x = scale0 * q1.x + scale1 * p1[0];
        y = scale0 * q1.y + scale1 * p1[1];
        z = scale0 * q1.z + scale1 * p1[2];
        w = scale0 * q1.w + scale1 * p1[3];
    }

    mathinline Quaternion operator*(const Quaternion& a, const Quaternion& b)
    {
        IncCounter(g_QuaternionMultiply);

        // See http://en.wikipedia.org/wiki/Quaternion#Quaternion_products
        return Quaternion
            (
                a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
                a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
                a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
                a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
            );
    }

    mathinline Quaternion operator+(const Quaternion& a, const Quaternion& b)
    {
        // See http://en.wikipedia.org/wiki/Quaternion#Common_binary_operations
        return Quaternion(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
    }

    mathinline void Quaternion::SetMatrix(const Matrix& m)
    {
        IncCounter(g_MatrixToQuaternionConversion);

        //See: http://gamedev.ru/articles/?id=30129&page=2
        float tr = m(0, 0) + m(1, 1) + m(2, 2); // trace of martix
        if (tr > 0.0f)
        {     
            // if trace positive than "w" is biggest component
            float w = tr+1.0f;
            float factor = 0.5f * sqrtInv(w);
            Set(factor * (m(2, 1) - m(1, 2)), 
                factor * (m(0, 2) - m(2, 0)),
                factor * (m(1, 0) - m(0, 1)),
                factor * w);

            return;
        }

        if ((m(0, 0) > m(1, 1)) && (m(0, 0) > m(2, 2)))
        {
            float x = 1.0f + m(0, 0) - m(1, 1) - m(2, 2);
            float factor = 0.5f * sqrtInv(x);
            Set(factor * x, 
                factor * (m(0, 1) + m(1, 0)),
                factor * (m(0, 2) + m(2, 0)),
                factor * (m(2, 1) - m(1, 2)));
            return;
        }

        if (m(1, 1) > m(2, 2))
        {
            float y = 1.0f + m(1, 1) - m(0, 0) - m(2, 2);
            float factor = 0.5f * sqrtInv(y);
            Set(factor * (m(0, 1) + m(1, 0)),
                factor * y,
                factor * (m(1, 2) + m(2, 1)),
                factor * (m(0, 2) - m(2, 0))); 
            return;
        }

        float z = 1.0f + m(2, 2) - m(0, 0) - m(1, 1);
        float factor = 0.5f * sqrtInv(z);
        Set(factor * (m(0, 2) + m(2, 0)), 
            factor * (m(1, 2) + m(2, 1)),
            factor * z, 
            factor * (m(1, 0) - m(0, 1)));
    }

    mathinline void Quaternion::GetAxisAngle(Vector& axis, Scalar& angle)
    {
        float vl = sqrt(x*x + y*y + z*z);
        if (vl > DEFAULT_EPSILON)
        {
            float ivl = 1.0f/vl;
            axis.Set(x * ivl, y * ivl, z * ivl);
            if (w < 0)
                angle = 2.0f*(float)atan2(-vl, -w); //-PI,0 
            else
                angle = 2.0f*(float)atan2( vl,  w); //0,PI 
        }
        else
        {
            axis.Set(0, 0, 0);
            angle = 0;
        }
    }

    mathinline void Quaternion::operator*=(const Quaternion& arg)
    {
        *this = *this * arg;
    }
}