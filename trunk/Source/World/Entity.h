#pragma once

#include "CollisionModel.h"

namespace P3D
{
    namespace World
    {
        using namespace P3D::Graphics;

        class CompoundEntity;
        class World;
        class RendererContext;
        class EntityController;

        /*
        Something in the world that has position and orientation and can render itself.
        */
        class Entity : 
            public Object,
            public ObjectWithTags
        {
            friend class CompoundEntity;
            friend class World;
            friend class Camera;
            friend class Controller;

        public:
            // Available entity classes
            enum EntityClass
            {
                Entity_World,
                Entity_Compound,
                Entity_Simple,
                Entity_Camera
            };

            Entity(World* parent);
            virtual ~Entity();

            /*
            Return entity class.
            */
            virtual EntityClass GetClass() const { return Entity_Simple; }

            /*
            Push current matrix.
            Load transform matrix.
            call DoRender
            Pop matrix.
            */
            void Render(const RendererContext& params);

            /*
            Called after world has been build before first update.
            */
            virtual void Prepare();

            /*
            Simulate physics, do thinking etc.
            Return true in case update happend i.e. tick counter changed since last update.
            */
            virtual bool Update();

            /*
            Return entity that contains this entity.
            */
            inline CompoundEntity* GetParentEntity() const { return _parent; }

            /*
            Return world the entity is in.
            */
            inline World* GetWorld() { return _parentWorld; }

            /*
            Returns transform from objects space to world space.
            */
            inline const QTransform& GetTransformToWorldSpace() const { return _toWorldSpace(); }

            /*
            Changes object transform matrix so that transform to world space == transform.
            */
            void SetTransformToWorldSpace(const QTransform& transform);

            /*
            Return bounding box in the object space.
            */
            inline const AABB& GetBoundingBox() const { return _bbox(); }

            /*
            Return bounding box in the parent's object space.
            */
            inline const AABB& GetBoundingBoxInParentSpace() const  { return _bboxInPS(); }

            /*
            Return QTransform from object space to parent space.
            */
            inline const QTransform& GetTransform() const { return _objectTransform; }

            /*
            Set QTransform from object space to parent space.
            */
            inline void SetTransform(const QTransform& transform)
            {
                _objectTransform = transform;
                InvalidateObjectTransform();
            }

            /*
            Return matrix based transform from objects space to parent space.
            */
            inline const Transform& GetMatrixTransform() const { return _transform(); }

            /*
            Return matrix based transform from parent space to object space.
            */
            inline const Transform& GetInvMatrixTransform() const { return _invTransform(); }

            /*
            Says that entity should recalculate bounding box next time its required.
            */
            inline void InvalidateBoundingBox()
            { 
                _bbox.Invalidate();
                _bboxInPS.Invalidate();
                if (_parent) ((Entity*)_parent)->InvalidateBoundingBox();
            }

            /*
            Return entities random color (for debug purposes).
            */
            inline uint32 GetColor() { return _color; }

            /*
            Controls the visibility of the entity.
            */
            virtual void SetVisible(bool visible)
            {
                _visible = visible;
                InvalidateBoundingBox();
            }

            /*
            Is entity visible?
            */
            inline bool IsVisible() const { return _visible; }

            /*
            Sets entities controller. AddRefs the controller.
            By default each entity use controller provided by PhysicalWorld.
            */
            virtual void SetController(EntityController* controller);

            /*
            Get current active controller.
            */
            inline EntityController* GetController() const {  return _controller; }

            /*
            Set the collision model of the entity.
            */
            inline void SetCollisionModel(Physics::CollisionModel* model) { _collisionModel = model; }

            /*
            Return entities collision model if any.
            */
            inline Physics::CollisionModel* GetCollisionModel() const { return _collisionModel; }

            /*
            Return entity mass.
            */
            inline Scalar GetMass() const { return _mass; }

            /*
            Set entity mass.
            0 means infinity.
            */
            inline void SetMass(Scalar mass) { _mass = mass; }

        protected:
            /*
            Set's opaque pointer to some controller specific stuff.
            Should be called by EntityController only.
            */
            void SetControllerData(void* controllerData) { _controllerData = controllerData; }

            /*
            Return stored controller data.
            Should be called by EntityController only.
            */
            void* GetControllerData() { return _controllerData; }

            /*
            Multiply current OpenGL matrix by ObjectTransform matrix.
            */
            inline void ApplyTransform() const
            {
                GLfloat transformMatrix[4][4];
                GetMatrixTransform().ToOpenGLMatrix(transformMatrix);
                glMultMatrixf(&transformMatrix[0][0]);
            }

            /*
            Multiply current OpenGL matrix by ObjectTransform^-1 matrix.
            */
            inline void ApplyInvTransform() const
            {
                GLfloat invTransformMatrix[4][4];
                GetInvMatrixTransform().ToOpenGLMatrix(invTransformMatrix);
                glMultMatrixf(&invTransformMatrix[0][0]);
            }

            /*
            Render object in the object space.
            To render child objects call Render
            RendererContext in objects space.
            */
            virtual void DoRender(const RendererContext& params);

            /*
            Recalculate bounding box.
            */
            virtual void RecalculateBoundingBox(AABB& box)
            {
                if (!IsVisible())
                    box.SetImpossible();
                else
                    box.SetZero();
            }

            /*
            Was Update called during this tick?
            */
            bool NeedUpdate();

            /*
            Says that entity should recalculate transform to world space.
            */
            virtual void InvalidateTransformToWorldSpace() { _toWorldSpace.Invalidate(); }

            /*
            Call this when you change ObjectTransform to update OpenGL transformation matrix
            and other important stuff.
            */
            inline void InvalidateObjectTransform()
            { 
                _transform.Invalidate();
                _invTransform.Invalidate();
                InvalidateTransformToWorldSpace();
                _bboxInPS.Invalidate();
                if (_parent) ((Entity*)_parent)->InvalidateBoundingBox();
            }

        private:
            struct OpenGLMatrix
            {
                GLfloat Matrix[4][4];
            };

            /*
            Recalculate OpenGL transform matrix from ObjectTransform.
            */
            inline void RecalculateTransform(Transform& t)
            {
                t = Transform(GetTransform());
            }

            /*
            Recalculate OpenGL invert transform matrix from ObjectTransform.
            Reset dirty flag.
            */
            inline void RecalculateInvTransform(Transform& t)
            {
                t = GetMatrixTransform();
                t.Invert();
            }

            /*
            Recalculate QTransform from object space to world space.
            */
            void RecalculateTransformToWorld(QTransform& transform);

            /*
            Recalculates bounding box in parent space.
            */
            inline void RecalculateBoundingBoxInPS(AABB& bbox)
            {
                if (!IsVisible())
                    bbox.SetImpossible();
                else
                {
                    bbox = GetBoundingBox();
                    bbox.Transform(GetMatrixTransform());
                }
            }

        private:
            Lazy<Entity, Transform> _transform; // ObjectTransform in matrix form
            Lazy<Entity, Transform> _invTransform;  // _transform ^ -1
            Lazy<Entity, QTransform> _toWorldSpace; // transform from object space to world space
            Lazy<Entity, AABB> _bbox; // bounding box in object space
            Lazy<Entity, AABB> _bboxInPS; // bounding box in parent space

            QTransform _objectTransform;

            CompoundEntity* _parent; // parent entity
            World* _parentWorld; // the world this entity is in

            uint _lastUpdateTick; // last tick count of the timer
            bool _prepareCalled;

            uint32 _color; // random color of the entity
            bool _visible; // visibility status
            bool _solid;

            SmartPointer<EntityController> _controller; // object that controls entity position
            void* _controllerData;

            SmartPointer<Physics::CollisionModel> _collisionModel;
            Scalar _mass;
        };
    }
}

#include "CompoundEntity.h"
#include "RendererContext.h"