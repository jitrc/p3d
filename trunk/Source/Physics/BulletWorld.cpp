#include "Includes.h"
#include "BulletWorld.h"
#include "BulletController.h"
#include "BulletMathIterop.h"

namespace P3D
{
    namespace Physics
    {
        PhysicalWorld* CreatePhysicalWorld()
        {
            return new BulletWorld();
        }

        BulletWorld::BulletWorld()
        {
            _broadphase = NULL;
            _collisionConf = NULL;
            _dispatcher = NULL;
            _solver = NULL;
            _world = NULL;
        }

        BulletWorld::~BulletWorld()
        {
            delete _world;
            delete _solver;
            delete _dispatcher;
            delete _collisionConf;
            delete _broadphase;
        }

        void BulletWorld::Prepare(P3D::World::World* world)
        {
            // Build the broadphase

            //btVector3 worldAabbMin = Conv(world->GetWorldBounds().Points[0]);
            //btVector3 worldAabbMax = Conv(world->GetWorldBounds().Points[1]);
            //_broadphase = new btAxisSweep3(worldAabbMin, worldAabbMax);
            _broadphase = new btDbvtBroadphase();

            // Set up the collision configuration and dispatcher
            _collisionConf = new btDefaultCollisionConfiguration();
            _dispatcher = new btCollisionDispatcher(_collisionConf);

            // The actual physics solver
            _solver = new btSequentialImpulseConstraintSolver();

            // The world.
            _world = new btDiscreteDynamicsWorld(_dispatcher, _broadphase,
                _solver, _collisionConf);

            _world->setGravity(btVector3(0, 0, -9.8f));
        }

        /*
        Return new entity controller that simulate physics.
        */
        World::EntityController* BulletWorld::CreateEntityController(World::Entity* entity)
        {
            if (entity->GetClass() != World::Entity::Entity_Simple)
                return NULL;
            return new BulletController(this, entity);
        }

        /*
        Make simulation step.
        */
        void BulletWorld::Update(TimeCounter& time)
        {
            if (time.GetTotalTime() <  1000) return;
            _world->stepSimulation(time.GetLastTick() * 0.001f, 2);
        }
    }
}