#pragma once

#include "BulletCollisionModels.h"

namespace P3D
{
    namespace Physics
    {
        class BulletWorld : 
            public PhysicalWorld
        {
        public:
            BulletWorld();
            virtual ~BulletWorld();

            /*
            Called before first update.
            */
            override void Prepare(P3D::World::World* world);

            /*
            Return new entity controller that simulate physics.
            */
            override World::EntityController* CreateEntityController(World::Entity* entity);

            /*
            Make simulation step.
            */
            override void Update(TimeCounter& time);

            /*
            PhysicalWorld also acts as class factory for Collision models.
            */
            override CollisionModel* CreateBoxCollisionModel(Scalar sx, Scalar sy, Scalar sz)
            {
                return new BulletBoxCollisionModel(sx, sy, sz);
            }

            /*
            Return bullet dynamics world.
            */
            btDynamicsWorld* GetDynamicsWorld() const { return _world; }

        private:
            btBroadphaseInterface* _broadphase;
            btCollisionConfiguration* _collisionConf;
            btCollisionDispatcher* _dispatcher;
            btConstraintSolver* _solver;
            btDynamicsWorld* _world;
        };
    }
}