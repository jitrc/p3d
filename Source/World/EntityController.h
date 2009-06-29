#pragma once

namespace P3D
{
    namespace World
    {
        /*
        Controls updates of the entities.
        */
        class EntityController :
            public Object,
            public ObjectWithTags
        {
            friend class Entity;

        protected:
            /*
            Called when the controller gets assigned to the entity.
            */
            virtual void Bind(Entity* entity) {}

            /*
            Called when entity no longer wants this controller.
            */
            virtual void Unbind(Entity* entity) {}

            /*
            Update entity's ObjectTransform and possibly other parameters.
            */
            virtual void UpdateEntity(Entity* entity, const TimeCounter& time) {}

            /*
            Called before first update.
            */
            virtual void Prepare(Entity* entity) {}
        };
    }
}