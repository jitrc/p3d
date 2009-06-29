#pragma once 

namespace P3D
{
    class Transform;
    class Matrix;
    class Quaternion;

    /*
    3D vector class.
    */
    class Vector
    {
    public:
        static const Vector Zero;
        static const Vector GetRandomVector(Scalar length = 1.0);

        mathinline Vector();
        mathinline Vector(Scalar x, Scalar y, Scalar z);
        mathinline Vector(const double*);
        mathinline Vector(const float*);

        mathinline void operator=(const Vector& v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
        }

        mathinline void Set(Scalar x, Scalar y, Scalar z) { this->x = x; this->y = y; this->z = z; }
        mathinline void SetZero() { x = y = z = 0; }

        /* Element access */
        mathinline Scalar& operator() (int a) { return (&x)[a]; }
        mathinline const Scalar& operator() (int a) const { return (&x)[a]; }

        /* Accumulation operators */
        mathinline void operator+= (const Vector& a)
        {
            x += a(0); y += a(1); z += a(2);
        }

        mathinline void operator-= (const Vector& a)
        {
            x -= a(0); y -= a(1); z -= a(2);
        }

        mathinline void operator*= (Scalar a) { x *= a; y *= a; z *= a; }

        mathinline void SetSphericalCoordinates(Scalar latitude, Scalar longitude, Scalar radius);

        mathinline       Scalar* GetRealPointer() { return &x; }
        mathinline const Scalar* GetRealPointer() const { return &x; }

        mathinline void Invert () { x = -x; y = -y; z = -z; }

        mathinline Scalar Length() const;
        mathinline Scalar LengthInv() const;
        mathinline Scalar LengthSquared() const;

        mathinline void Normalize();
        mathinline Scalar NormalizeWithLength();

        /*
        this = a * b;
        */
        mathinline void SetMultiply(const Matrix& a, const Vector& b);

        /*
        Rotates the vector according to matrix.
        A should be ortogonal matrix!
        */
        mathinline void Transform(const Matrix& a);

        /*
        Rotates the vector according to the inverse matrix.
        A should be ortogonal matrix!
        */
        mathinline void InvTransform(const Matrix& a);

        /*
        Rotates the vector according to matrix.
        A should be ortogonal matrix!
        */
        mathinline void Transform(const Quaternion& a);

        /*
        Rotates the vector according to the inverse matrix.
        A should be ortogonal matrix!
        */
        mathinline void InvTransform(const Quaternion& a);

        /*
        Translate and rotates the vector according to Transform.
        */
        mathinline void Transform(const P3D::Transform& a);

        /*
        Translate and rotates the vector according to invert Transform.
        */
        mathinline void InvTransform(const P3D::Transform& a);

        /*
        this = (1-t) * v1 + t * v2
        */
        mathinline void SetInterpolation(const Vector& v1, const Vector& v2, Scalar t);

    public:
        ALIGNED_VARIABLE(16, Scalar, x); // for SSE to work :-[
        Scalar y;
        Scalar z;
        Scalar w; // w for cool sizeof(Scalar)*4 size =)
    };

    mathinline const Vector operator+ (const Vector& a, const Vector& b);
    mathinline const Vector operator- (const Vector& a, const Vector& b);
    mathinline const Vector operator* (const Vector& b, const Scalar a);
    mathinline const Vector operator* (const Scalar a, const Vector& b);
    mathinline const Vector operator- (const Vector& a);
    mathinline const Scalar operator * (const Vector& a, const Vector& b);

    // Cross product operator
    mathinline const Vector operator ^ (const Vector& a, const Vector& b);
}