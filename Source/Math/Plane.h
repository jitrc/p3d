#pragma once

namespace P3D
{
    /*
    Plane is a normal + distance to origin.
    A*x + B*y + C*z + D = 0
    A = n.x, B = n.y, C = n.z, D = Distance
    */
    class Plane
    {
    public:
        mathinline Plane() {};
        mathinline Plane(const Plane& p) { *this = p; };
        mathinline Plane(const Vector& normal, Scalar distance) : _normal(normal)
        {
            _normal.w = distance;
        }
        mathinline Plane(Scalar nx, Scalar ny, Scalar nz, Scalar distance)
        {
            _normal.x = nx;
            _normal.y = ny;
            _normal.z = nz;
            _normal.w = distance;
        }

        mathinline void operator=(const Plane& p) { _normal = p._normal; _normal.w = p._normal.w; }

        mathinline void SetNormal(const Vector& n) { _normal = n; }
        mathinline const Vector& GetNormal() const { return _normal; }
        
        mathinline Scalar GetDistance() const { return _normal.w; }
        mathinline void SetDistance(Scalar distance) { _normal.w = distance; }

        mathinline void Set(const Vector& normal, Scalar distance) { _normal = normal; _normal.w = distance; }

        /*
        Construct plane that contains triangle (a, b, c).
        */
        mathinline void Construct(const Vector& a, const Vector& b, const Vector& c)
        {
            _normal = (a - b) ^ (c - b);
            _normal.Normalize();
            _normal.w = - _normal * b;
        }

        /*
        Return v*n + d.
        If it is positive than point is in positive halfspace.
        If it is negative than point is in negative halfspace.
        If it is equal to zero than point is on plane.
        */
        mathinline Scalar TestPoint(const Vector& v) const
        {
            return v.x*_normal.x + v.y*_normal.y + v.z*_normal.z + _normal.w;
        }

        /*
        Transform plane to new coordinate frame.
        */
        mathinline void Transform(const QTransform& q);

        /*
        Transform plane to new coordinate frame.
        */
        mathinline void Transform(const P3D::Transform& q);

        /*
        Find intersection point of three planes.
        Return true in case there is one and only one intersection.
        */
        static mathinline bool GetIntersection(Vector& res, const Plane& p1, const Plane& p2, const Plane& p3);

    protected:
        // Use w component of the vector as distance.
        Vector _normal;
    };
}