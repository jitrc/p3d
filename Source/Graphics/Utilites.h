#pragma once

#include "Math/Frustum.h"
#include "Math/Sphere.h"
#include "Math/AABB.h"

namespace P3D
{
    namespace Graphics
    {
        void InitializeText();
        void OutputText(float x, float y, float z, const char* format, ...);

        void DrawFrame();
        void DrawFrustum(const Frustum& f);
        void DrawSphere(const Sphere& sphere);
        void DrawAABB(const AABB& box);
    }
}