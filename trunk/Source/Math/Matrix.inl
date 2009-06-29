#pragma once

namespace P3D
{
    mathinline Matrix::Matrix()
    {
#ifdef TD_MATH_PARANOIA
        elements[0] = elements[4] = elements[11] = 1000000.0f;
#endif
    }

    mathinline Matrix::Matrix(Scalar m00, Scalar m01, Scalar m02, 
                      Scalar m10, Scalar m11, Scalar m12, 
                      Scalar m20, Scalar m21, Scalar m22)
    {
        SetColumns(Vector(m00, m01, m02), 
                   Vector(m10, m11, m12),
                   Vector(m20, m21, m22));
    }

    mathinline Vector& Matrix::GetColumn(int x) { return *(Vector*)&elements[4*x]; };
    mathinline const Vector& Matrix::GetColumn(int x) const  { return *(const Vector*)&elements[4*x]; }

    mathinline void Matrix::Transpose()
    {
        Scalar swap;
        swap = GetColumn(0)(1);    GetColumn(0)(1) = GetColumn(1)(0); GetColumn(1)(0) = swap;
        swap = GetColumn(0)(2);    GetColumn(0)(2) = GetColumn(2)(0); GetColumn(2)(0) = swap;
        swap = GetColumn(1)(2);    GetColumn(1)(2) = GetColumn(2)(1); GetColumn(2)(1) = swap;        
    }

    mathinline void Matrix::SetRows(const Vector& r0, const Vector& r1, const Vector& r2)
    {
        GetColumn(0).Set( r0(0), r1(0), r2(0) );
        GetColumn(1).Set( r0(1), r1(1), r2(1) );
        GetColumn(2).Set( r0(2), r1(2), r2(2) );
    }

    mathinline void Matrix::SetColumns(const Vector& c0, const Vector& c1, const Vector& c2)
    {
        GetColumn(0) = c0;
        GetColumn(1) = c1;
        GetColumn(2) = c2;
    }

    mathinline void Matrix::SetZero()
    {
        GetColumn(0).SetZero();
        GetColumn(1).SetZero();
        GetColumn(2).SetZero();
    }

    mathinline void Matrix::SetDiagonal(Scalar d0, Scalar d1, Scalar d2)
    {
        GetColumn(0).Set(d0, 0, 0);
        GetColumn(1).Set( 0,d1, 0);
        GetColumn(2).Set( 0, 0,d2);
    }

    mathinline void Matrix::SetIdentity()
    {
        SetDiagonal(1, 1, 1);
    }

    mathinline void Matrix::SetCrossSkewSymmetric(const Vector& v)
    {
        // See http://en.wikipedia.org/wiki/Cross_product#Conversion_to_matrix_multiplication
        GetColumn(0).Set(0, -v.z, v.y);
        GetColumn(1).Set(v.z, 0, -v.x);
        GetColumn(2).Set(-v.y, v.x, 0);
    }

    mathinline bool Matrix::Invert(Scalar epsilon)
    {
        IncCounter(g_MatrixInverse);

        Vector r0 = GetColumn(1) ^ GetColumn(2);
        Vector r1 = GetColumn(2) ^ GetColumn(0);
        Vector r2 = GetColumn(0) ^ GetColumn(1);

        Scalar D = GetColumn(0) * r0;    // main determinant
        
        if (abs(D) < epsilon)
            return false; 

        Scalar DI = Scalar(1.0f)/D;
        r0 *= DI;
        r1 *= DI;
        r2 *= DI;

        SetRows(r0, r1, r2);
        return true;
    }

    mathinline void Matrix::operator+=(const Matrix& a)
    {
        GetColumn(0) += a.GetColumn(0);
        GetColumn(1) += a.GetColumn(1);
        GetColumn(2) += a.GetColumn(2);
    }

    mathinline void Matrix::operator-=(const Matrix& a)
    {
        GetColumn(0) -= a.GetColumn(0);
        GetColumn(1) -= a.GetColumn(1);
        GetColumn(2) -= a.GetColumn(2);
    }

    mathinline void Matrix::operator*=(Scalar a)
    {
        GetColumn(0) *= a;
        GetColumn(1) *= a;
        GetColumn(2) *= a;
    }

    mathinline void Matrix::operator*=(const Matrix& a)
    {
        IncCounter(g_MatrixMultiply);

        Matrix& Ma = *this;
        for (int i = 0; i < 3; i++)
        {
            Scalar x = Ma(0,0)*a(0,i) + Ma(0,1)*a(1,i) + Ma(0,2)*a(2,i);
            Scalar y = Ma(1,0)*a(0,i) + Ma(1,1)*a(1,i) + Ma(1,2)*a(2,i);
            Scalar z = Ma(2,0)*a(0,i) + Ma(2,1)*a(1,i) + Ma(2,2)*a(2,i);

            GetColumn(i).Set(x,y,z);
        }
    }

    mathinline void Matrix::SetMultiply(const Matrix& a, const Matrix& b)
    {
        IncCounter(g_MatrixMultiply);

        for (int i = 0; i < 3; i++)
        {
            Scalar x = a(0,0)*b(0,i) + a(0,1)*b(1,i) + a(0,2)*b(2,i);
            Scalar y = a(1,0)*b(0,i) + a(1,1)*b(1,i) + a(1,2)*b(2,i);
            Scalar z = a(2,0)*b(0,i) + a(2,1)*b(1,i) + a(2,2)*b(2,i);

            GetColumn(i).Set(x,y,z);
        }
    }

    mathinline void Matrix::SetQuaternion(const Quaternion& q)
    {
        // See http://en.wikipedia.org/wiki/Rotation_matrix#Quaternion

        IncCounter(g_QuaternionToMatrixConversion);

        Scalar wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
        x2 = q.x + q.x;
        y2 = q.y + q.y;
        z2 = q.z + q.z;
        xx = q.x * x2;   xy = q.x * y2;   xz = q.x * z2;
        yy = q.y * y2;   yz = q.y * z2;   zz = q.z * z2;
        wx = q.w * x2;   wy = q.w * y2;   wz = q.w * z2;

        GetColumn(0).Set(1.0f-(yy+zz), xy-wz, xz+wy);
        GetColumn(1).Set(xy+wz, 1.0f-(xx+zz), yz-wx);
        GetColumn(2).Set(xz-wy, yz+wx, 1.0f-(xx+yy));
    }

    mathinline void Matrix::SetAxisAngle(const Vector& axis, Scalar angle)
    {
        SetQuaternion(Quaternion(axis, angle));
    }

    mathinline void Matrix::SetEulerAngles(Scalar yaw, Scalar pitch, Scalar roll)
    {
        // See http://en.wikipedia.org/wiki/Euler_angles#Table_of_matrices

        // x y z
        Scalar c1, s1;
        sincos(roll, &s1, &c1);
        Scalar c2, s2;
        sincos(pitch, &s2, &c2);
        Scalar c3, s3;
        sincos(yaw, &s3, &c3);

        GetColumn(0).Set(c2 * c3, c3 * s1 *s2 - c1 * s3, c1 * c3 * s2 + s1 * s3 );
        GetColumn(1).Set(c2 * s3, c1 * c3 + s1 * s2 * s3, c1 * s2 * s3 - c3 * s1);
        GetColumn(2).Set(-s2, c2 * s1, c1 * c2 );
    }

    mathinline Scalar Matrix::GetDeterminant() const
    {
        const Matrix& m = *this;
        return  m(0, 0) * (m(1, 1) * m(2, 2) - m(2, 1) * m(1, 2)) -
                m(0, 1) * (m(1, 0) * m(2, 2) - m(2, 0) * m(1, 2)) +
                m(0, 2) * (m(1, 0) * m(2, 1) - m(2, 0) * m(1, 1));
    }
}