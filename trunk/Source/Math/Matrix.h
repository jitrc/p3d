#pragma once 

namespace P3D
{
    /*
    3x3 matrix.
    All operations do not check range.
    */
    class Matrix
    {
    public:
        const static Matrix Identity;

        mathinline Matrix();

        mathinline Matrix(Scalar m00, Scalar m01, Scalar m02, 
                          Scalar m10, Scalar m11, Scalar m12, 
                          Scalar m20, Scalar m21, Scalar m22);

        /*
        Initialize from a normalized quaternion.
        */
        mathinline explicit Matrix(const Quaternion& q) { SetQuaternion(q); }

        /*
        Initialize from rotation axis and angle
        */
        mathinline Matrix(const Vector& axis, Scalar angle) { SetAxisAngle(axis, angle); }

        /*
        Initialize from euler angles.
        */
        mathinline Matrix(Scalar yaw, Scalar pitch, Scalar roll) { SetEulerAngles(yaw, pitch, roll); }

        mathinline Vector& GetColumn(int x);
        mathinline const Vector& GetColumn(int x) const;

        mathinline void SetRow(int row, const Vector& v)
        {
            GetColumn(0)(row) = v(0);
            GetColumn(1)(row) = v(1);
            GetColumn(2)(row) = v(2);
        }

        mathinline void SetElement(int x, int y, Scalar r) { GetColumn(x)(y) = r; }
        

        mathinline Scalar operator()(int x, int y) { return GetColumn(x)(y); }
        mathinline Scalar operator()(int x, int y) const { return GetColumn(x)(y); }

        mathinline void SetRows(const Vector& r0, const Vector& r1, const Vector& r2);
        mathinline void SetColumns(const Vector& c0, const Vector& c1, const Vector& c2);

        mathinline void SetZero();
        mathinline void SetDiagonal(Scalar d0, Scalar d1, Scalar d2);
        mathinline void SetIdentity();

        /*
        Constructs matrix [v] with property:
        v ^ a === [v]*a
        See http://en.wikipedia.org/wiki/Cross_product#Conversion_to_matrix_multiplication
        */
        mathinline void SetCrossSkewSymmetric(const Vector& v);

        /*
        Transpose the matrix inplace.
        */
        mathinline void Transpose();

        /*
        Invert matrix inplace.
        Return false in case the matrix is not invertable.
        */
        mathinline bool Invert(Scalar epsilon = DEFAULT_EPSILON);

        mathinline void operator+=(const Matrix& a);
        mathinline void operator-=(const Matrix& a);
        mathinline void operator*=(const Matrix& a);
        mathinline void operator*=(Scalar a);
        
        /*
        this = a * b
        */
        mathinline void SetMultiply(const Matrix& a, const Matrix& b);

        /*
        Set from a normalized quaternion.
        */
        mathinline void SetQuaternion(const Quaternion& q);

        /*
        Set rotation to be 'angle' radians about the vector 'axis'
        */
        mathinline void SetAxisAngle(const Vector& axis, Scalar angle);

        /*
        Set rotation from euler angles.
        Its actually Tait-Bryan rotations (x y z order).
        */
        mathinline void SetEulerAngles(Scalar yam, Scalar pitch, Scalar roll);

        /*
        Return determinant of the 3x3 matrix.
        */
        mathinline Scalar GetDeterminant() const;

    public:
        ALIGNED_VARIABLE(16, Scalar, elements[12]);
    };
}