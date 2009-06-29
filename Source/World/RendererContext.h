#pragma once

namespace P3D
{
    namespace World
    {
        enum RendererFlags
        {
            RF_RenderHelpers = 1 << 1, // draw frame axis and bounding boxes
            RF_RenderAll     = 1 << 2, // do not perform frustum culling
        };

        /*
        The state of the renderer during rendering.
        */
        class RendererContext
        {
        public:
            /*
            Constructs new fresh context.
            */
            RendererContext(const Frustum& frustum);

            /*
            Derived context to render entity.
            */
            RendererContext(const RendererContext* parent, const Entity* entity);

            /*
            Return current frustum.
            */
            const Frustum& GetFrustum() const;

        public:
            int Flags;

        private:
            const RendererContext* _parent;
            const Entity* _entity;

            mutable bool _hasFrustum;
            mutable Frustum _frustum;
        };
    }
}