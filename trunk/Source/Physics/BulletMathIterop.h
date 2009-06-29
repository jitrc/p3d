#pragma  once

namespace P3D
{
    namespace Physics
    {
        mathinline const btVector3 Conv(const Vector& v)
        {
            return btVector3(v.x, v.y, v.z);
        }

        mathinline const Vector Conv(const btVector3& v)
        {
            return Vector(v.x(), v.y(), v.z());
        }

        mathinline const btQuaternion Conv(const Quaternion& q)
        {
            return btQuaternion(q.x, q.y, q.z, q.w);
        }

        mathinline const Quaternion Conv(const btQuaternion& q)
        {
            return Quaternion(q.x(), q.y(), q.z(), q.w());
        }

        mathinline const btTransform Conv(const QTransform& tr)
        {
            btTransform t;
            t.setOrigin(Conv(tr.Translation));
            t.setRotation(Conv(tr.Rotation));
            return t;
        }

        mathinline const QTransform Conv(const btTransform& tr)
        {
            QTransform t;
            t.Translation = Conv(tr.getOrigin());
            t.Rotation = Conv(tr.getRotation());
            return t;
        }
    }
}