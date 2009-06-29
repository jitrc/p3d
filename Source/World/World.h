#pragma once

#include "Entity.h"
#include "CompoundEntity.h"
#include "Camera.h"
#include "PhysicalWorld.h"

namespace P3D
{
    namespace World
    {
        class World : 
            public CompoundEntity
        {
            friend class Entity;

        public:
            /*
            Construct empty world with given physical engine.
            Borrows reference to physicalWorld.
            */
            World(Physics::PhysicalWorld* physicalWorld);

            /*
            Destructor.
            */
            override ~World();

            /*
            Return entity class.
            */
            override EntityClass GetClass() const { return Entity_World; }

            /*
            Called after world has been build before first update.
            */
            override void Prepare();

            /*
            Update world state before rendering (physics, thinking etc.)
            */
            override bool Update();

            /*
            Renders world from camera viewpoint.
            */
            void Render(Camera* camera);

            /*
            Return global time counter.
            */
            const TimeCounter& Time() const { return _timeCounter; }

            /*
            Return camera we are rendering from.
            */
            const Camera* GetActiveCamera() const { return _activeCamera; }

            /*
            Return associated physical world.
            */
            Physics::PhysicalWorld* GetPhysicalWorld() const { return _physicalWorld; }

            /*
            Return AABB that surely encloses all world at any moment of time.
            */
            const AABB& GetWorldBounds() const { return _worldBounds; }

            /*
            Set AABB that surely encloses all world at any moment of time.
            Defaults is (-1000, -1000, -1000) - (1000, 1000, 1000)
            */
            void SetWorldBounds(const AABB& aabb) { _worldBounds = aabb; }

        private:
            void Render(const RendererContext& params) { CompoundEntity::Render(params); } // hide from pubic members

        private:
            TimeCounter _timeCounter; // counts msecs
            SmartPointer<Camera> _activeCamera;
            SmartPointer<Physics::PhysicalWorld> _physicalWorld;
            AABB _worldBounds;
        };
    }
}