#pragma once

namespace P3D
{
    class Sphere
    {
    public:
        mathinline Sphere() {};
        mathinline Sphere(const Sphere& sphere) { *this = sphere; }
        mathinline Sphere(const Vector& center, Scalar radius) { _c = center; _c.w = radius; }

        mathinline void operator=(const Sphere& s) { _c = s._c; _c.w = s._c.w; }

        mathinline const Vector& GetCenter() const { return _c; }
        mathinline void SetCenter(const Vector& v) { _c = v; }

        mathinline Scalar GetRadius() const { return _c.w; }
        mathinline void SetRadius(Scalar c) { _c.w = c; }

        mathinline void Transform(const QTransform& q)
        {
            _c.Transform(q.Rotation);
            _c += q.Translation;
        }

        mathinline void Transform(const P3D::Transform& q)
        {
            _c.Transform(q.Rotation);
            _c += q.Translation;
        }

    protected:
        Vector _c;
    };
}