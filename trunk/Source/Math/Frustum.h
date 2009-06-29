#pragma once

namespace P3D
{
    /*
    For the implementation details of the frustum culling algorithm refer to   
    http://www.cescg.org/CESCG-2002/DSykoraJJelinek/index.html                 
    */

    /*
    Plane of the frustum.
    Contains normal indexing values.
    */
    class FrustumPlane : public Plane
    {
    public:
        /*
        Recalculate nx, ny, nz, px, py, pz from normal.
        */
        mathinline void CalculateNormalIndices()
        {
            if (_normal.x > 0)
            {
                px = 1;
                nx = 0;
            } else
            {
                px = 0;
                nx = 1;
            }

            if (_normal.y > 0)
            {
                py = 1;
                ny = 0;
            } else
            {
                py = 0;
                ny = 1;
            }

            if (_normal.z > 0)
            {
                pz = 1;
                nz = 0;
            } else
            {
                pz = 0;
                nz = 1;
            }
        }
    public:
        int nx, ny, nz;
        int px, py, pz;
    };


    /*
    6 planes.
    Near, far, left, right, bottom and top.
    Defines viewalbe volume.
    The planes normals is pointed inside the frustum.
    The point is visible if its plane.Test(...) returns positive for all six planes.
    */
    class Frustum
    {
    public:
        enum Planes
        {
            NEAR_PLANE,
            FAR_PLANE,
            LEFT_PLANE,
            RIGHT_PLANE,
            UP_PLANE,
            DOWN_PLANE
        };

        mathinline Frustum()
        {
            _count = 0;
        }

        /*
        Return count of planes in the frustum.
        */
        mathinline int GetPlaneCount() const { return _count; }

        /*
        Return specific plane.
        */
        mathinline const Plane& operator()(int i) const
        {
            ASSERT(i < _count);
            return _planes[i];
        }

        /*
        Constructs frustrum for the camera.
        'fov' is full FOV (from left to right, not the half).
        aspectRation is screen width / sceen height.
        View vector is (-1.0, 0, 0).
        */
        mathinline void Construct(Scalar fov, Scalar aspectRatio, Scalar near, Scalar far);

        /*
        Constructs shadow (infinite non-caped) volume for the box. The view point should be outside the box.
        */
        mathinline void ConstructShadow(const Vector& lightPoint, const AABB& box);

        /*
        Computes and return bounding sphere of the simple frustum created by Construct call.
        */
        mathinline Sphere GetBoundingSphere() const;

        /*
        Transform the frustum to new coordinate frame.
        */
        mathinline void Transform(const QTransform& q)
        {
            this->Transform(P3D::Transform(q));
        }

        /*
        Transform the frustum to new coordinate frame.
        */
        mathinline void Transform(const P3D::Transform& t)
        {
            for (int i = 0; i < _count; i++)
            {
                _planes[i].Transform(t);
                _planes[i].CalculateNormalIndices();
            }
        }

        /*
        Does frustum volume intersects with sphere?
        */
        mathinline IntersectionResult Intersects(const Sphere& s) const; 

        /*
        Does frustum intersects AABB?
        */
        mathinline IntersectionResult Intersects(const AABB& box) const;

        /*
        Does frustum intersects AABB?
        Use plane masking as described in http://www.cescg.org/CESCG-2002/DSykoraJJelinek/index.html
        Suitable for hierarchy.
        */
        mathinline IntersectionResult Intersects(const AABB& box, int inMask, int& outMask) const;

    public:
        static const int MAX_FRUSTUM_PLANES = 32; // sizeof(int)*8... related to mask in Intersects
        int _count;
        FrustumPlane _planes[MAX_FRUSTUM_PLANES];
        
    protected:
        mathinline void RecalculateNormalIndices()
        {
            for (int i = 0; i < _count; i++)
                _planes[i].CalculateNormalIndices();
        }
    };
}