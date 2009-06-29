#pragma once

#include "VertexBuffer.h"
#include "VertexDescription.h"

namespace P3D
{
    namespace Graphics
    {
        enum PrimitiveType
        {
            PRIMITIVE_POINTS = GL_POINTS,
            PRIMITIVE_LINES = GL_LINES,
            PRIMITIVE_LINE_LOOP = GL_LINE_LOOP,
            PRIMITIVE_LINE_STRIP = GL_LINE_STRIP,
            PRIMITIVE_TRIANGLES = GL_TRIANGLES,
            PRIMITIVE_TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
            PRIMITIVE_TRIANGLE_FAN = GL_TRIANGLE_FAN,
            PRIMITIVE_QUADS = GL_QUADS,
            PRIMITIVE_QUAD_STRIP = GL_QUAD_STRIP,
            PRIMITIVE_POLYGON = GL_POLYGON
        };

        /*
        Vertex buffer, index buffer, vertex format and primitive type.
        Can effectivelly draw itself using VBO.
        */
        class Primitive : public Object
        {
        public:
            Primitive() : _type(PRIMITIVE_TRIANGLE_STRIP) {}
            virtual ~Primitive() {};

            /*
            Set the vertex buffer. AddRef it.
            */
            void SetVertexBuffer(VertexBuffer* vb) { _vb = vb; }
            VertexBuffer* GetVertexBuffer() const { return _vb; }

            /*
            Set the index buffer. AddRef it.
            */
            void SetIndexBuffer(IndexBuffer* ib, uint indexSize = 0)
            {
                _ib = ib;
                if (ib) indexSize = ib->Element.Size;
                _indexSize = indexSize;
            }
            IndexBuffer* GetIndexBuffer() const { return _ib; }

            /*
            Set the primitive type. Default is PRIMITIVE_TRIANGLE_STRIP.
            indexType is a type of index.
            offset is offset of the indices in the IB expressed in indeces (not bytes).
            count is amount of indices to render.
            */
            void SetType(PrimitiveType type, int offset, int count)
            {
                _type = type;
                _offset = offset;
                _count = count;
            }
            PrimitiveType GetType() const { return _type; }
            int GetIndexOffset() const { return _offset; }
            int GetIndexCount() const { return _count; }

            /*
            Draw the primitive using current OpenGL state.
            */
            void Render();

            /*
            Renders primitive without creating instance.
            Pass vb == NULL or ib == NULL to use already bound buffer.
            In case ib == NULL you should provide size of the index in 'elementSize' parameter.
            */
            static void Render(PrimitiveType type, int offset, int count, 
                VertexBuffer* vb, IndexBuffer* ib, uint elementSize = 0);

        protected:
            SmartPointer<VertexBuffer> _vb;
            SmartPointer<IndexBuffer> _ib;
            PrimitiveType _type;
            int _count;
            int _offset;
            int _indexSize;
        };
    }
}