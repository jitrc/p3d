#pragma once

#include "Entity.h"

namespace P3D
{
    namespace World
    {
        /*
        Compound entity can contain entities inside.
        These entities is located in CompoundEntitys object space.
        */
        class CompoundEntity : public Entity
        {
        public:
            CompoundEntity(World* world);
            virtual ~CompoundEntity();

            /*
            Return entity class.
            */
            override EntityClass GetClass() const { return Entity_Compound; }

            /*
            Add entity to the childs list.
            Ignore it if it is already there.
            Removes it from former parent list.
            Do not AddRefs it but takes ownership.
            */
            virtual void AddEntity(Entity* entity);

            /*
            Remove entity from the childs list (releasing it).
            */
            virtual void RemoveEntity(Entity* entity);

            /*
            Remove all entities from the list calling RemoveEntity method.
            */
            void RemoveAllEntities();

            /*
            Return true in case the entity is among childs.
            */
            bool Contains(Entity* entity) const;

            /*
            Simulate physics, do thinking etc.
            */
            override bool Update();

            /*
            Called after world has been build before first update.
            */
            override void Prepare();

        protected:
            /*
            Render all entites one by one in order they were added.
            */
            override void DoRender(const RendererContext& params);

            /*
            Says that entity should recalculate transform to world space.
            */
            override void InvalidatTransformToWorldSpace();

            /*
            Recalculate bounding box.
            */
            override void RecalculateBoundingBox(AABB& box);

        protected:
            typedef std::list<Entity*> ChildsContainer;
            ChildsContainer _childs;
        };
    }
}