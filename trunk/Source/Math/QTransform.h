#pragma once

namespace P3D
{
    class Transform;

    /*
    Quaternion based transform.
    */
    class QTransform
    {
    public:
        mathinline QTransform() {};

        /*
        Constructs QTransform from translation vector and rotation quaternion.
        */
        mathinline QTransform(const Vector& translate, const Quaternion& rotate) 
            : Translation(translate), Rotation(rotate)
        { }

        mathinline QTransform(const QTransform& t)
        {
            *this = t;
        }

        /*
        Constructs QTransform from Transform.
        */
        mathinline explicit QTransform(const Transform& t);

        mathinline void operator=(const QTransform& t)
        {
            Rotation = t.Rotation;
            Translation = t.Translation;
        }

        mathinline void SetIdentity() 
        {
            Rotation.SetIdentity();
            Translation.SetZero();
        }

        mathinline void Invert()
        {
            Rotation.Invert();
            Translation.Invert();
            Translation.Transform(Rotation);
        }

        /*
        Converts transform to OpenGL 4x4 matrix that
        can be loaded by glLoadMatrix or glMultMatrix.
        */
        mathinline void ToOpenGLMatrix(float m[4][4])
        {
            // Return T*R

            Matrix r(Rotation);
            Vector& t = Translation;

            m[0][0] = r(0, 0);
            m[1][0] = r(0, 1);
            m[2][0] = r(0, 2);
            m[3][0] = t(0);

            m[0][1] = r(1, 0);
            m[1][1] = r(1, 1);
            m[2][1] = r(1, 2);
            m[3][1] = t(1);

            m[0][2] = r(2, 0);
            m[1][2] = r(2, 1);
            m[2][2] = r(2, 2);
            m[3][2] = t(2);

            m[0][3] = 0;
            m[1][3] = 0;
            m[2][3] = 0;
            m[3][3] = 1;
        }

        /*
        Converts transform invers to this to OpenGL 4x4 matrix that
        can be loaded by glLoadMatrix or glMultMatrix.
        */
        mathinline void ToInverseOpenGLMatrix(float m[4][4]) const
        {
            // return R^-1 * T^-1

            Matrix r(Rotation);

            Vector nt(-Translation.x, -Translation.y, -Translation.z);
            nt.InvTransform(r);

            m[0][0] = r(0, 0);
            m[1][0] = r(1, 0);
            m[2][0] = r(2, 0);
            m[3][0] = nt.x;

            m[0][1] = r(0, 1);
            m[1][1] = r(1, 1);
            m[2][1] = r(2, 1);
            m[3][1] = nt.y;

            m[0][2] = r(0, 2);
            m[1][2] = r(1, 2);
            m[2][2] = r(2, 2);
            m[3][2] = nt.z;

            m[0][3] = 0;
            m[1][3] = 0;
            m[2][3] = 0;
            m[3][3] = 1;
        }

    public:
        Quaternion Rotation;
        Vector Translation;
    };

    /*
    Result of t1*t2 is transform that first applies t2 than t1.
    */
    mathinline QTransform operator*(const QTransform& t1, const QTransform& t2)
    {
        // new translation vector
        Vector trans(t2.Translation);
        trans.Transform(t1.Rotation);
        trans += t1.Translation;
        return QTransform(trans, t1.Rotation * t2.Rotation);
    }
}