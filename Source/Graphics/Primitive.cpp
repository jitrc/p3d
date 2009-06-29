#include "Includes.h"
#include "Primitive.h"
#include "Utilites.h"

namespace P3D
{
    namespace Graphics
    {
        void Primitive::Render()
        {
            // bind both buffers

            if (_vb) _vb->Activate();
            if (_ib) _ib->Bind();

            // find index type by its size
            GLenum indexType;
            switch (_indexSize)
            {
            case 1: indexType = GL_UNSIGNED_BYTE; break;
            case 2: indexType = GL_UNSIGNED_SHORT; break;
            case 4: indexType = GL_UNSIGNED_INT; break;
            default:
                ASSERT(false && "Unsupported index type");
            }

            // render the primitives
            glDrawElements(_type, _count, indexType, (GLvoid*)(_offset * _indexSize));

            if (_ib) _ib->Unbind();
            if (_vb) _vb->Deactivate();

            switch (_type)
            {
            case PRIMITIVE_TRIANGLES: g_PolygonCounter += _count/3; break;
            case PRIMITIVE_TRIANGLE_STRIP: g_PolygonCounter += (_count - 2); break;
            case PRIMITIVE_TRIANGLE_FAN: g_PolygonCounter += (_count - 2); break;
            case PRIMITIVE_QUADS: g_PolygonCounter += _count/4; break;
            case PRIMITIVE_QUAD_STRIP: g_PolygonCounter += 1 + (_count - 2)/2; break;
            case PRIMITIVE_POLYGON: g_PolygonCounter++; break;
            }
        }

        void Primitive::Render(PrimitiveType type, int offset, int count, 
            VertexBuffer* vb, IndexBuffer* ib, uint elementSize)
        {
            if (vb) vb->Activate();
            if (ib)
            {
                ib->Bind();
                elementSize = ib->Element.Size;
            }

            GLenum indexType;
            switch (elementSize)
            {
            case 1: indexType = GL_UNSIGNED_BYTE; break;
            case 2: indexType = GL_UNSIGNED_SHORT; break;
            case 4: indexType = GL_UNSIGNED_INT; break;
            default:
                ASSERT(false && "Unsupported index type");
            }

            // render the primitives
            glDrawElements(type, count, indexType, (GLvoid*)(offset * elementSize));

            if (ib) ib->Unbind();
            if (vb) vb->Deactivate();

            switch (type)
            {
            case PRIMITIVE_TRIANGLES: g_PolygonCounter += count/3; break;
            case PRIMITIVE_TRIANGLE_STRIP: g_PolygonCounter += (count - 2); break;
            case PRIMITIVE_TRIANGLE_FAN: g_PolygonCounter += (count - 2); break;
            case PRIMITIVE_QUADS: g_PolygonCounter += count/4; break;
            case PRIMITIVE_QUAD_STRIP: g_PolygonCounter += 1 + (count - 2)/2; break;
            case PRIMITIVE_POLYGON: g_PolygonCounter++; break;
            }
        }
    }
}