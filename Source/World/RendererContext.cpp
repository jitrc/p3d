#include "Includes.h"
#include "Entity.h"

namespace P3D
{
    namespace World
    {
        RendererContext::RendererContext(const Frustum& frustum) :
            _parent(NULL), _hasFrustum(true), _frustum(frustum), _entity(NULL),
            Flags(0)
        { }

        RendererContext::RendererContext(const RendererContext* parent, const Entity* entity) :
            _parent(parent), _hasFrustum(false), _entity(entity), Flags(parent->Flags)
        { }

        const Frustum& RendererContext::GetFrustum() const
        {
            ASSERT(_parent!=NULL || _hasFrustum);
            if (!_hasFrustum) 
            {
                _frustum = _parent->GetFrustum();
                _frustum.Transform(_entity->GetInvMatrixTransform());
                _hasFrustum = true;
            }
            return _frustum;
        }
    }
}

