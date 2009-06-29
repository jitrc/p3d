#pragma once

namespace P3D
{
    namespace World
    {
        class EntityController;
        class Entity;
        class World;
    }

    namespace Physics
    {
        class CollisionModel;

        /*
        Base abstract class for physics sub-system.
        */
        class PhysicalWorld :
            public Object
        {
        public:
            /*
            Return new entity controller that simulate physics.
            */
            virtual World::EntityController* CreateEntityController(World::Entity* entity) = 0;

            /*
            Called before first update.
            */
            virtual void Prepare(P3D::World::World* world) = 0;

            /*
            Make simulation step.
            */
            virtual void Update(TimeCounter& time) = 0;

            /*
            PhysicalWorld also acts as class factory for Collision models.
            */
            virtual CollisionModel* CreateBoxCollisionModel(Scalar sx, Scalar sy, Scalar sz) = 0;
        };
    }
}