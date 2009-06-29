#pragma once

namespace P3D
{
    mathinline Transform::Transform(const QTransform& t)
        : Translation(t.Translation), Rotation(t.Rotation)
    { }

    mathinline QTransform::QTransform(const Transform& t)
        : Translation(t.Translation), Rotation(t.Rotation)
    { }
}