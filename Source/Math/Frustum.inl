#pragma once

#include "Vector.h"
#include "Plane.h"
#include "Frustum.h"


namespace P3D
{
    mathinline bool Plane::GetIntersection(Vector& res, const Plane& p1, const Plane& p2, const Plane& p3)
    {
        // Solve linear 3x3 system using cramer method
        Vector d(-p1.GetDistance(), -p2.GetDistance(), -p3.GetDistance());
        Matrix m;

        m.SetColumns(p1.GetNormal(), p2.GetNormal(), p3.GetNormal());
        Scalar det = m.GetDeterminant();
        if (abs(det) < DEFAULT_EPSILON) return false;

        m.SetColumns(p1.GetNormal(), p2.GetNormal(), p3.GetNormal());
        m.SetRow(0, d);
        res.x = m.GetDeterminant() / det;

        m.SetColumns(p1.GetNormal(), p2.GetNormal(), p3.GetNormal());
        m.SetRow(1, d);
        res.y = m.GetDeterminant() / det;

        m.SetColumns(p1.GetNormal(), p2.GetNormal(), p3.GetNormal());
        m.SetRow(2, d);
        res.z = m.GetDeterminant() / det;

        return true;
    }

    mathinline void Plane::Transform(const QTransform& q)
    {
        _normal.Transform(q.Rotation); // rotate normal
        _normal.w -= (_normal * q.Translation);
    }

    mathinline void Plane::Transform(const P3D::Transform& q)
    {
        _normal.Transform(q.Rotation); // rotate normal
        _normal.w -= (_normal * q.Translation);
    }

    mathinline void Frustum::Construct(Scalar fov, Scalar aspectRatio, Scalar nearPlane, Scalar farPlane)
    {
        _count = 6;

        // easy part: set near and far planes
        _planes[NEAR_PLANE].Set(Vector( -1.0f, 0, 0), -nearPlane);
        _planes[FAR_PLANE].Set(Vector(1.0f, 0, 0), farPlane);

        Scalar s, c;
        sincos(fov * 0.5f, &s, &c);

        // set up and down planes
        _planes[DOWN_PLANE].Set(Vector(-s, 0, c), 0);
        _planes[UP_PLANE].Set(Vector(-s, 0, -c), 0);

        Scalar y = s * aspectRatio;
        Scalar x = - c;
        Scalar norm = sqrtInv(x*x + y*y);
        y *= norm;
        x *= norm;

        _planes[LEFT_PLANE].Set(Vector(-y, -x, 0), 0);
        _planes[RIGHT_PLANE].Set(Vector(-y, x, 0), 0);

        RecalculateNormalIndices();
    }

    Sphere Frustum::GetBoundingSphere() const
    {
        Vector dl;
        Vector ur;

        if (!Plane::GetIntersection(dl, _planes[LEFT_PLANE], _planes[DOWN_PLANE], _planes[FAR_PLANE])) return Sphere(Vector(0, 0, 0), 0);
        if (!Plane::GetIntersection(ur, _planes[RIGHT_PLANE], _planes[UP_PLANE], _planes[FAR_PLANE])) return Sphere(Vector(0, 0, 0), 0);;

        Scalar dist = - ur.x;
        ur -= dl;
        Scalar side = ur.Length() * 0.5f;
        Scalar radius = 0.5f * (dist + side * side / dist);

        return Sphere(Vector(-radius, 0, 0), radius);
    }

    mathinline IntersectionResult Frustum::Intersects(const Sphere& s) const
    {
        Scalar dis;
        bool inside = true;

        for (int i = 0; i < _count; i++)
        {
            // distance to the sphere center
            dis = _planes[i].TestPoint(s.GetCenter()); 
            if (dis < -s.GetRadius()) return OUTSIDE; // too far
            inside = inside && (dis > s.GetRadius());
        }

        return inside ? INSIDE : INTERSECTS;
    }

    /*
    Does frustum intersects AABB?
    */
    mathinline IntersectionResult Frustum::Intersects(const AABB& box) const
    {
        Scalar m, n;
        IntersectionResult result = INSIDE;

        for (int i = 0; i < _count; i++)
        {  
            const FrustumPlane& plane = _planes[i];
            const Vector& normal = plane.GetNormal();  
            m = (normal.x * box.Points[plane.px].x) +  
                (normal.y * box.Points[plane.py].y) +  
                (normal.z * box.Points[plane.pz].z);  
            if (m < -plane.GetDistance()) return OUTSIDE;  
            n = (normal.x * box.Points[plane.nx].x) +  
                (normal.y * box.Points[plane.ny].y) +  
                (normal.z * box.Points[plane.nz].z);  
            if (n < -plane.GetDistance()) result = INTERSECTS;  
        }

        return result;
    }

    mathinline IntersectionResult Frustum::Intersects(const AABB& box, int inMask, int& outMask) const
    {
        Scalar m, n;
        IntersectionResult result = INSIDE;
        int k = 1;
        outMask = 0;

        for (int i = 0; i < _count; i++, k += k)
        {
            if (k > inMask) return result; 
            if (k & inMask) 
            { 
                const FrustumPlane& plane = _planes[i];
                const Vector& normal = plane.GetNormal(); 
                m = (normal.x * box.Points[plane.px].x) +  
                    (normal.y * box.Points[plane.py].y) +  
                    (normal.z * box.Points[plane.pz].z);  
                if (m < -plane.GetDistance()) return OUTSIDE;  
                n = (normal.x * box.Points[plane.nx].x) +  
                    (normal.y * box.Points[plane.ny].y) +  
                    (normal.z * box.Points[plane.nz].z);  
                if (n < -plane.GetDistance())  
                {  
                    outMask |= k;  
                    result = INTERSECTS;  
                }  
            }
        }

        return result;
    }

    /*
    Constructs shadow frustum for the box.
    */
    mathinline void Frustum::ConstructShadow(const Vector& lightPoint, const AABB& box)
    {
        ASSERT(!box.IsInside(lightPoint));

        // precalculated silhouettes of box for specific light position class
        static const char silhouette[27][8] = 
        {
            { 1, 3, 2, 6, 4, 5, 1,-1 },
            { 0, 4, 5, 1, 3, 2, 0,-1 }, 
            { 0, 4, 5, 7, 3, 2, 0,-1 },
            { 2, 6, 4, 0, 1, 3, 2,-1 },
            { 0, 1, 3, 2, 0,-1,-1,-1 },
            { 0, 1, 5, 7, 3, 2, 0,-1 },
            { 0, 1, 3, 7, 6, 4, 0,-1 },
            { 0, 1, 3, 7, 6, 2, 0,-1 },
            { 0, 1, 5, 7, 6, 2, 0,-1 },
            { 0, 2, 6, 4, 5, 1, 0,-1 },
            { 0, 4, 5, 1, 0,-1,-1,-1 },
            { 0, 4, 5, 7, 3, 1, 0,-1 },
            { 0, 2, 6, 4, 0,-1,-1,-1 },
            {-1,-1,-1,-1,-1,-1,-1,-1 }, // inside
            { 1, 5, 7, 3, 1,-1,-1,-1 },
            { 0, 2, 3, 7, 6, 4, 0,-1 },
            { 2, 3, 7, 6, 2,-1,-1,-1 },
            { 2, 3, 1, 5, 7, 6, 2,-1 },
            { 0, 2, 6, 7, 5, 1, 0,-1 },
            { 0, 4, 6, 7, 5, 1, 0,-1 },
            { 0, 4, 6, 7, 3, 1, 0,-1 },
            { 0, 2, 6, 7, 5, 4, 0,-1 },
            { 4, 6, 7, 5, 4,-1,-1,-1 },
            { 1, 5, 4, 6, 7, 3, 1,-1 },
            { 2, 3, 7, 5, 4, 0, 2,-1 },
            { 3, 7, 5, 4, 6, 2, 3,-1 },
            { 1, 5, 4, 6, 2, 3, 1,-1 },
        };                      

        // get apropriate contour
        const char* contour = silhouette[box.ClassifyPoint(lightPoint)];
        _count = 0;
        Vector prev;
        Vector cur;

        box.GetPoint(*contour, cur);
        cur -= lightPoint;
        contour++;
        while (*contour >= 0)
        {
            prev = cur;
            box.GetPoint(*contour, cur);
            cur -= lightPoint;
            contour++;

            Vector normal = prev ^ cur;
            normal.Normalize();
            _planes[_count].Set(normal, - lightPoint * normal);
            _planes[_count].CalculateNormalIndices();
            _count++;
        }
    }
}