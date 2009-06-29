#include "Includes.h"
#include "CompoundEntity.h"

namespace P3D
{
    namespace World
    {
        CompoundEntity::CompoundEntity(World* world)
            : Entity(world)
        {
        }

        CompoundEntity::~CompoundEntity()
        {
            RemoveAllEntities();
            ASSERT(_childs.empty());
        }

        void CompoundEntity::AddEntity(Entity* entity)
        {
            ASSERT(entity->GetWorld() == GetWorld()); // from same worlds :)
            ASSERT(entity->_parent == NULL);

            if (Contains(entity)) return;
            entity->_parent = this;
            _childs.push_back(entity);

            InvalidateBoundingBox();
        }

        void CompoundEntity::RemoveEntity(Entity* entity)
        {
            ChildsContainer::iterator it = std::find(_childs.begin(), _childs.end(), entity);
            if (it == _childs.end()) return; // no such child
            _childs.erase(it);
            entity->_parent = NULL;
            entity->Release();

            InvalidateBoundingBox();
        }

        void CompoundEntity::RemoveAllEntities()
        {
            while (!_childs.empty())
                RemoveEntity(_childs.back());
        }

        bool CompoundEntity::Contains(Entity* entity) const
        {
            ChildsContainer::const_iterator it = std::find(_childs.begin(), _childs.end(), entity);
            return (it != _childs.end());
        }

        void CompoundEntity::DoRender(const RendererContext& params)
        {
            for (ChildsContainer::const_iterator it = _childs.begin(); it != _childs.end(); ++it)
                (*it)->Render(params);
            Entity::DoRender(params);
        }

        bool CompoundEntity::Update()
        {
            if (Entity::Update())
            {
                for (ChildsContainer::const_iterator it = _childs.begin(); it != _childs.end(); ++it)
                    (*it)->Update();
                return true;
            } else
                return false;
        }

        void CompoundEntity::Prepare()
        {
            Entity::Prepare();
            for (ChildsContainer::const_iterator it = _childs.begin(); it != _childs.end(); ++it)
                (*it)->Prepare();
        }

        void CompoundEntity::InvalidatTransformToWorldSpace()
        {
            Entity::InvalidateTransformToWorldSpace();

            // and that all childs info is out of date
            for (ChildsContainer::iterator it = _childs.begin(); it != _childs.end(); ++it)
                (*it)->InvalidateTransformToWorldSpace();
        }

        void CompoundEntity::RecalculateBoundingBox(AABB& box)
        {
            box.SetImpossible();
            for (ChildsContainer::iterator it = _childs.begin(); it != _childs.end(); ++it)
            {
                if (!(*it)->IsVisible()) continue;
                box.Enlarge((*it)->GetBoundingBoxInParentSpace());
            }
        }
    }
}