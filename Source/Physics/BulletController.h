#pragma once

#include "BulletWorld.h"

namespace P3D
{
    namespace Physics
    {
        class BulletController :
            public World::EntityController,
            protected btMotionState
        {
            static Logger logger;
        public:
            BulletController(BulletWorld* world, World::Entity* entity);

            /*
            Called when the controller gets assigned to the entity.
            */
            override void Bind(World::Entity* entity);

            /*
            Called when entity no longer wants this controller.
            */
            override void Unbind(World::Entity* entity);

            /*
            Update entity's ObjectTransform and possibly other parameters.
            */
            override void UpdateEntity(World::Entity* entity, const TimeCounter& time);

            /*
            Called before first update.
            */
            override void Prepare(World::Entity* entity);

        protected:
            // From btMotionState
            override void getWorldTransform(btTransform& worldTrans) const;

            //Bullet only calls the update of worldtransform for active objects
            override void setWorldTransform(const btTransform& worldTrans);

        private:
            btRigidBody* _body;
            World::Entity* _entity;
            mutable btTransform _toWorldSpace;
        };
    }
}