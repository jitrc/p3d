#pragma once

#include "Entity.h"

namespace P3D
{
    namespace World
    {
        /*
        Camera can loads camera transform into OpenGL.
        */
        class Camera : public Entity
        {
        public:
            Camera(World* world);

            /*
            Return entity class.
            */
            override EntityClass GetClass() const { return Entity_Camera; }

            /*
            Loads projection & transform matrix into OpenGL system.
            (replaces current matrices).
            Take into account that camera can be inside CompoundEntity.
            */
            void LoadCameraTransform() const;

            /*
            Get field of view value expressed in radians.
            */
            inline float GetFOV() const { return _fov;}

            /*
            Set field of view value expressed in radians.
            */
            inline void SetFOV(float fov) { _fov = fov; _frustum.Invalidate(); }

            /*
            Get 'screen plane width' / 'screen plane height'.
            */
            inline float GetAspectRatio() const { return _aspectRatio; }

            /*
            Set 'screen plane width' / 'screen plane height'.
            */
            inline void SetAspectRatio(float aspectRatio) { _aspectRatio = aspectRatio; _frustum.Invalidate(); }

            /*
            Get near projection plane.
            */
            inline float GetNearPlane() const { return _nearPlane; }

            /*
            Set near projection plane.
            */
            inline void SetNearPlane(float nearPlane) { _nearPlane = nearPlane; _frustum.Invalidate(); }

            /*
            Get far projection plane.
            */
            inline float GetFarPlane() const { return _farPlane; }

            /*
            Set far projection plane.
            */
            inline void SetFarPlane(float farPlane) { _farPlane = farPlane; _frustum.Invalidate(); }

            /*
            Get coresponding Frustum object in camera space.
            */
            inline const Frustum& GetFrustum() const { return _frustum(); }
            
            /*
            Return sphere (in object space) that encloses all frustrum.
            */
            inline const Sphere& GetBoundingSphere() const { _frustum.Update(); return _sphere; }

            /*
            Rotate camera so it looks at specific direction.
            */
            void LookAt(const Vector& direction, const Vector& up = Vector(0, 0, 1));

        protected:
            virtual void DoRender(const RendererContext& params);

        private:
            void RecalculateFrustum(Frustum& frustum)
            {
                frustum.Construct(_fov, _aspectRatio, _nearPlane, _farPlane);
                _sphere = frustum.GetBoundingSphere();
            }

        protected:
            float _fov;
            float _aspectRatio;
            float _nearPlane, _farPlane;

            Lazy<Camera, Frustum> _frustum;
            Sphere _sphere;
        };
    }
}