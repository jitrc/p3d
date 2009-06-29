#pragma once

namespace P3D
{
    /*
    Basic quaternion class.
    Support only multiplication.
    */
    class Quaternion
    {
    public:
        /*
        Constructs quaternion with garbage values.
        */
        mathinline Quaternion();

        /*
        Copy constructor.
        */
        mathinline Quaternion(const Quaternion& q) { *this = q; }
        
        /*
        Vector-to-Quaternion constructor.
        */
        mathinline Quaternion(const Vector& v) { Set(v.x, v.y, v.z, (Scalar)0); }

        /*
        Construct quaternion from values.
        */
        mathinline Quaternion(Scalar x, Scalar y, Scalar z, Scalar w);

        /*
        Construct quaternion from euler angles.
        */
        mathinline Quaternion(Scalar yaw, Scalar pitch, Scalar roll) { SetEulerAngles(yaw, pitch, roll); }

        /*
        Constructs normalized rotation quaternion from axis and rotation angle.
        */
        mathinline Quaternion(const Vector& axis, Scalar angle);

        /*
        Constructs quaternion from rotation matrix. Matrix should be ortogonal.
        */
        mathinline Quaternion(const Matrix& m) { SetMatrix(m); }

        mathinline void operator=(const Quaternion& q) { x = q.x; y = q.y; z = q.z; w = q.w; }

        /*
        Sets all quaternion components.
        */
        mathinline void Set(Scalar x, Scalar y, Scalar z, Scalar w);

        /*
        Sets quaternion to identity quaternion (0, 0, 0, 1).
        */
        mathinline void SetIdentity();

        /*
        Sets normalized rotation quaternion from axis and rotation angle.
        Axis should be normalized!
        */
        mathinline void SetAxisAngle(const Vector& axis, Scalar angle);

        /*
        Set rotation from euler angles.
        Its actually Tait-Bryan rotations (z-y-x order).
        */
        mathinline void SetEulerAngles(Scalar yaw, Scalar pitch, Scalar roll);

        /*
        Normalizes the quaternion.
        */
        mathinline void Normalize();

        /*
        Return length of the quaternion.
        */
        mathinline Scalar Length() const { return sqrt(x*x + y*y + z*z + w*w); }

        /*
        Make inverse rotation.
        Quaternion should be normalized!
        */
        mathinline void Invert() { x = -x; y = -y; z = -z; }

        /* Element access */
        mathinline Scalar& operator() (int a) { return (&x)[a]; }
        mathinline const Scalar& operator() (int a) const { return (&x)[a]; }

        mathinline       Scalar* GetRealPointer() { return &x; }
        mathinline const Scalar* GetRealPointer() const { return &x; }

        mathinline void SetReal(Scalar r) { w = r; }
        mathinline Scalar GetReal() const { return w; }

        mathinline void SetImaginary(const Vector& v) { x = v.x; y = v.y; z = v.z; }
        mathinline const Vector& GetImaginary() const { return *((const Vector*)this); } 

        mathinline void SetMatrix(const Matrix& m);

        mathinline void GetAxisAngle(Vector& axis, Scalar& angle);

        /*
        Set this quaternion to slerp from q1 to q2.
        t is in range [0, 1]
        */
        mathinline void SetInterpolation(const Quaternion& q1, const Quaternion& q2, Scalar t);

        mathinline void operator*=(const Quaternion& arg);

        mathinline void operator+=(const Quaternion& arg) { x += arg.x;    y += arg.y;    z += arg.z;    w += arg.w; }
    public:
        ALIGNED_VARIABLE(16, Scalar, x); // for SSE to work :-[
        Scalar y;
        Scalar z;
        Scalar w; 
    };

    mathinline Quaternion operator*(const Quaternion& arg1, const Quaternion& arg2);
    mathinline Quaternion operator+(const Quaternion& arg1, const Quaternion& arg2);
}