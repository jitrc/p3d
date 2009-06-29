#pragma once

namespace P3D
{
    /*
    Axis-Aligned Bounding Box.
    */
    class AABB
    {
    public:
        mathinline AABB() {}
        mathinline AABB(const Vector& min, const Vector& max)
        {
            Min() = min;
            Max() = max;
        }

        /*
        Make box bigger so that it can include vertex v.
        */
        mathinline void Enlarge(const Vector& v)
        {
            if (Min().x > v.x) Min().x = v.x;
            if (Max().x < v.x) Max().x = v.x;

            if (Min().y > v.y) Min().y = v.y;
            if (Max().y < v.y) Max().y = v.y;

            if (Min().z > v.z) Min().z = v.z;
            if (Max().z < v.z) Max().z = v.z;
        }

        /*
        Make box bigger so that it can include other box.
        */
        mathinline void Enlarge(const AABB& box) 
        {
            // TODO: optimize to safe some cmps ?...
            Enlarge(box.Min());
            Enlarge(box.Max());
        }

        /*
        Is vector inside the box?
        */
        mathinline bool IsInside(const Vector& v) const 
        {
            if (v.x < Min().x) return false;
            if (v.x > Max().x) return false;
            if (v.y < Min().y) return false;
            if (v.y > Max().y) return false;
            if (v.z < Min().z) return false;
            if (v.z > Max().z) return false;
            return true;
        }

        mathinline void SetZero()
        {
            Min().SetZero();
            Max().SetZero();
        }

        /*
        Set Max < Min and Max and Min has very large values.
        */
        mathinline void SetImpossible()
        {
            Min().Set(1e12f, 1e12f, 1e12f);
            Max().Set(-1e12f, -1e12f, -1e12f);
        }

        mathinline Vector& Min() { return Points[0]; }
        mathinline Vector& Max() { return Points[1]; }

        mathinline const Vector& Min() const { return Points[0]; }
        mathinline const Vector& Max() const { return Points[1]; }


        /*
        Fills array with corner points of box. Array should be at least 8 elements size.
        Index has meaning.... implementation.
        */
        mathinline void GetPoints(Vector* pointsArray) const 
        { 
            #define FILL_POINT(px, py, pz) pointsArray[(px | (py << 1) | (pz << 2))].Set(Points[px].x, Points[py].y, Points[pz].z);
            FILL_POINT(0, 0, 0);
            FILL_POINT(1, 0, 0);
            FILL_POINT(0, 1, 0);
            FILL_POINT(1, 1, 0);
            FILL_POINT(0, 0, 1);
            FILL_POINT(1, 0, 1);
            FILL_POINT(0, 1, 1);
            FILL_POINT(1, 1, 1);
            #undef FILL_POINT
        }

        mathinline void GetPoint(int index, Vector& p) const
        {
            int px = (index & 0x1);
            int py = (index & 0x2) >> 1;
            int pz = (index & 0x4) >> 2;
            p.Set(Points[px].x, Points[py].y, Points[pz].z);
        }

        /*
        Classify point according to the box...
        See implementation for details.
        Used in Frustum::ConstructShadow
        */
        mathinline int ClassifyPoint(const Vector& p) const
        {
            // classify position of the lightPoint
            int posClass = 0;

            if (p.x < Min().x) 
                posClass += 0;
            else if (p.x > Max().x)
                posClass += 2;
            else
                posClass += 1;
                
            if (p.y < Min().y) 
                posClass += 0;
            else if (p.y > Max().y)
                posClass += 2 * 3;
            else
                posClass += 1 * 3;

            if (p.z < Min().z) 
                posClass += 0;
            else if (p.z > Max().z)
                posClass += 2 * 9;
            else
                posClass += 1 * 9;

            return posClass;
        }

        /*
        Transform the AABB to new coordinate frame.
        */
        mathinline void Transform(const QTransform& q)
        {
            this->Transform(P3D::Transform(q));
        }

        /*
        Transform the AABB to new coordinate frame.
        */
        mathinline void Transform(const P3D::Transform& t)
        {
            Vector points[8];
            GetPoints(points);
            SetImpossible();
            for (uint i = 0; i < 8; i++)
            {
                points[i].Transform(t);
                Enlarge(points[i]);
            }
        }

        /*
        Copy operator.
        */
        mathinline const AABB& operator=(const AABB& b)
        {
            Points[0] = b.Points[0];
            Points[1] = b.Points[1];
            return *this;
        }

    public:
        Vector Points[2]; // Points[0] - min, Points[1] - max
    };
}