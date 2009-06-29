#include "Includes.h"
#include "Entity.h"
#include "World.h"

#include "EntityController.h"

namespace P3D
{
    namespace World
    {
        Entity::Entity(World* world) : 
            _transform(this, &Entity::RecalculateTransform),
            _invTransform(this, &Entity::RecalculateInvTransform),
            _toWorldSpace(this, &Entity::RecalculateTransformToWorld),
            _bbox(this, &Entity::RecalculateBoundingBox),
            _bboxInPS(this, &Entity::RecalculateBoundingBoxInPS),

            _parentWorld(world),
            _parent(NULL),
            _lastUpdateTick(0), _visible(true),
            _controller(NULL), _controllerData(NULL),
            _prepareCalled(false), _mass(0.0f)
        {
            _objectTransform.SetIdentity();

            _color = 0;
            _color |= (byte)random(50, 255);
            _color |= (byte)random(50, 255) << 8;
            _color |= (byte)random(50, 255) << 16;
            _color |= (byte)random(50, 255) << 24;
        }

        Entity::~Entity()
        {
            if (_controller)
            {
                _controller->Unbind(this);
                _controller = NULL;
                _controllerData = NULL;
            }
        }

        void Entity::SetTransformToWorldSpace(const QTransform& transform)
        {
            if (GetParentEntity())
            {
                QTransform parentToWorld = GetParentEntity()->GetTransformToWorldSpace();
                parentToWorld.Invert();
                SetTransform(parentToWorld * transform);
            } else
                SetTransform(transform);
            _toWorldSpace.Set(transform);
        }

        bool Entity::NeedUpdate()
        {
            uint ticks = GetWorld()->Time().GetTicks();
            if (ticks == _lastUpdateTick) return false; // already simulated
            return true;
        }

        bool Entity::Update()
        {
            if (!_prepareCalled) Prepare();
            uint ticks = GetWorld()->Time().GetTicks();
            if (ticks == _lastUpdateTick) return false; // already simulated
            _lastUpdateTick = ticks;
            if (_controller)
                _controller->UpdateEntity(this, GetWorld()->Time());
            return true;
        }

        void Entity::RecalculateTransformToWorld(QTransform& transform)
        {
            if (_parent)
            {
                const QTransform& parentToWorld = _parent->GetTransformToWorldSpace();
                transform = parentToWorld * GetTransform();
            } else
                transform = GetTransform(); // we are at root
        }

        void Entity::Render(const RendererContext& params)
        {
            if (!IsVisible()) return;
            if (((params.Flags & RF_RenderAll) != 0) ||
                params.GetFrustum().Intersects(GetBoundingBoxInParentSpace()) != OUTSIDE)
            {
                IncCounter(g_EntitiesRendered);

                glPushMatrix();
                ApplyTransform();

                RendererContext context(&params, this);
                DoRender(context);

                glPopMatrix();
            }
        }

        void Entity::DoRender(const RendererContext& params) 
        { 
            if (params.Flags & RF_RenderHelpers)
            {
                Graphics::DrawFrame();
                glColor3ubv((GLubyte*)&_color);
                Graphics::DrawAABB(GetBoundingBox());
            }
        }

        void Entity::SetController(EntityController* controller)
        {
            if (controller == _controller) return;

            if (_controller)
            {
                _controller->Unbind(this);
                _controllerData = NULL;
            }
            _controller = controller;
            if (_controller)
                _controller->Bind(this);
        }

        void Entity::Prepare()
        {
            if (_prepareCalled) return;
            _prepareCalled = true;
            if (_controller == NULL)
            {
                if (_collisionModel != NULL)
                {
                    Physics::PhysicalWorld* physicalWorld = _parentWorld->GetPhysicalWorld();
                    if (physicalWorld)
                    {
                        EntityController* controller = physicalWorld->CreateEntityController(this);
                        SetController(controller);
                        if (controller) controller->Release();
                    }
                }
            }
            if (_controller)
                _controller->Prepare(this);
        }
    }
}