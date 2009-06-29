#pragma once

#include "BulletMathIterop.h"

namespace P3D
{
    namespace Physics
    {
        class BulletBoxCollisionModel : 
            public CollisionModel,
            public btBoxShape
        {
        public:
            BulletBoxCollisionModel(Scalar sx, Scalar sy, Scalar sz)
                : CollisionModel(CollisionModel::Box),
                    btBoxShape(btVector3(sx / 2, sy / 2, sz / 2))
            { }
        };
    }
}