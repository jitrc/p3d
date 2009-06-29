#pragma once

namespace P3D
{
    mathinline const Vector Vector::GetRandomVector(Scalar length)
    {
        Vector vec((Scalar)random(-1.0, 1.0), (Scalar)random(-1.0, 1.0), (Scalar)random(-1.0, 1.0));
        vec.Normalize();
        return vec * length;
    }

    mathinline Vector::Vector()
    {
#ifdef TD_MATH_PARANOIA
        x = y = z = w = 1000000.0f;
#endif
    }

    mathinline Vector::Vector(Scalar a, Scalar b, Scalar c)
        : x(a), y(b), z(c)
    { }

    mathinline Vector::Vector(const double* vec)
        : x(Scalar(vec[0])), y(Scalar(vec[1])), z(Scalar(vec[2]))
    { }

    mathinline Vector::Vector(const float* vec) 
        : x(Scalar(vec[0])), y(Scalar(vec[1])), z(Scalar(vec[2]))
    { }

    mathinline Scalar Vector::Length() const
    {
        return sqrt(x*x + y*y + z*z);
    }

    mathinline Scalar Vector::LengthInv() const
    {
        return sqrtInv(x*x + y*y + z*z);
    }

    mathinline Scalar Vector::LengthSquared() const
    {
        return x*x + y*y + z*z;
    }

    mathinline void Vector::Normalize()
    {
        *this *= LengthInv();
    }

    mathinline Scalar Vector::NormalizeWithLength()
    {
        Scalar len = Length();
        *this *= Scalar(1.0) / len;
        return len;
    }

    mathinline void Vector::SetSphericalCoordinates(Scalar latitude, Scalar longitude, Scalar radius)
    {
        Scalar slat, clat;
        Scalar slong, clong;

        sincos(latitude, &slat, &clat);
        sincos(longitude, &slong, &clong);

        x = clat * clong * radius;
        y = clat * slong * radius;
        z = slat * radius;
    }

    mathinline void Vector::SetMultiply(const Matrix& a, const Vector& b)
    {
        IncCounter(g_VectorMatrixTransform);

        Scalar nx = a(0,0)*b(0) + a(0,1)*b(1) + a(0,2)*b(2);
        Scalar ny = a(1,0)*b(0) + a(1,1)*b(1) + a(1,2)*b(2);
        Scalar nz = a(2,0)*b(0) + a(2,1)*b(1) + a(2,2)*b(2);
        x = nx;
        y = ny;
        z = nz;
    }

    mathinline void Vector::Transform(const class Matrix& a)
    {
        SetMultiply(a, *this);
    }

    mathinline void Vector::InvTransform(const class Matrix& a)
    {
        IncCounter(g_VectorMatrixTransform);

        Vector& b = *this;

        Scalar nx = a(0,0)*b(0) + a(1,0)*b(1) + a(2,0)*b(2);
        Scalar ny = a(0,1)*b(0) + a(1,1)*b(1) + a(2,1)*b(2);
        Scalar nz = a(0,2)*b(0) + a(1,2)*b(1) + a(2,2)*b(2);
        x = nx;
        y = ny;
        z = nz;
    }

    mathinline void Vector::Transform(const P3D::Transform& trans)
    {
        IncCounter(g_VectorMatrixTransform);

        Vector& b = *this;
        const Matrix& a = trans.Rotation;
        const Vector& t = trans.Translation;

        Scalar nx = a(0,0)*b(0) + a(0,1)*b(1) + a(0,2)*b(2) + t(0);
        Scalar ny = a(1,0)*b(0) + a(1,1)*b(1) + a(1,2)*b(2) + t(1);
        Scalar nz = a(2,0)*b(0) + a(2,1)*b(1) + a(2,2)*b(2) + t(2);
        x = nx;
        y = ny;
        z = nz;
    }

    /*
    Translate and rotates the vector according to invert Transform.
    */
    mathinline void Vector::InvTransform(const P3D::Transform& t)
    {
        IncCounter(g_VectorMatrixTransform);

        const Matrix& r = t.Rotation;
        Vector& v = *this;

        Scalar v0 = v(0) - t.Translation(0);
        Scalar v1 = v(1) - t.Translation(1);
        Scalar v2 = v(2) - t.Translation(2);

        Scalar a = r(0,0)*v0 + r(1,0)*v1 + r(2,0)*v2;
        Scalar b = r(0,1)*v0 + r(1,1)*v1 + r(2,1)*v2;
        Scalar c = r(0,2)*v0 + r(1,2)*v1 + r(2,2)*v2;
        x = a;
        y = b; 
        z = c;
    }

    mathinline void Vector::Transform(const Quaternion& a)
    {
        this->Transform(Matrix(a));
    }

    mathinline void Vector::InvTransform(const Quaternion& a)
    {
        this->InvTransform(Matrix(a));
    }

    mathinline const Vector operator+ (const Vector& a, const Vector& b)
    {
        return Vector(a.x + b.x, a.y + b.y, a.z + b.z);
    }

    mathinline const Vector operator- (const Vector& a, const Vector& b)
    {
        return Vector(a.x - b.x, a.y - b.y, a.z - b.z);
    }

    mathinline const Vector operator* (const Vector& b, const Scalar a)
    {
        return Vector(a * b.x, a * b.y, a * b.z);
    }

    mathinline const Vector operator* (const Scalar a, const Vector& b)
    {
        return Vector(a * b.x, a * b.y, a * b.z);
    }

    mathinline const Vector operator- (const Vector& a)
    {
        return Vector(-a.x, -a.y, -a.z);
    }

    mathinline const Scalar operator * (const Vector& a, const Vector& b)
    {
        return a(0) * b(0) + a(1) * b(1) + a(2) * b(2);
    }

    // Cross product operator
    mathinline const Vector operator ^ (const Vector& a, const Vector& b)
    {
        return Vector(a(1) * b(2) - a(2) * b(1),
                      a(2) * b(0) - a(0) * b(2),
                      a(0) * b(1) - a(1) * b(0));
    }

    mathinline void Vector::SetInterpolation(const Vector& v1, const Vector& v2, Scalar t)
    {
        *this = (Scalar(1.0) - t) * v1 + t * v2;
    }
}