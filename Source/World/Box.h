#pragma once

#include "Entity.h"

namespace P3D
{
    namespace World
    {
        class Box : public Entity
        {
        public:
            Box(World* world, Scalar dx, Scalar dy, Scalar dz);

        protected:
            override void RecalculateBoundingBox(AABB& box);
            override void DoRender(const RendererContext& context);

        private:
            Scalar _dx, _dy, _dz;
        };
    }
}