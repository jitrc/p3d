#pragma once

#include "VertexDescription.h"

namespace P3D
{
    namespace Graphics
    {
        enum BufferType
        {
            BUFFER_STATIC = GL_STATIC_DRAW,
            BUFFER_DYNAMIC = GL_DYNAMIC_DRAW,
            BUFFER_STREAM = GL_STREAM_DRAW
        };

        enum BufferAccess
        {
            BUFFER_ACCESS_DRAW = 0,
            BUFFER_ACCESS_READ = 1,
            BUFFER_ACCESS_COPY = 2
        };

        namespace Implementation
        {
            extern void InitArrays(const VertexDescription& vd);
            extern void DeinitArrays(const VertexDescription& vd);

            template<int target>
            class OpenGLBuffer : public Object
            {
            public:
                OpenGLBuffer()
                {
                    _size = 0;
                    glGenBuffersARB(1, &_id);
                }

                virtual ~OpenGLBuffer()
                {
                    glDeleteBuffersARB(1, &_id);
                    _id = 0;
                }

                void Bind()
                {
                    glBindBufferARB(target, _id);
                }

                void Unbind()
                {
                    glBindBufferARB(target, NULL);
                }

                void Activate()
                {
                    Bind();
                    InitArrays(Element);
                }

                void Deactivate()
                {
                    DeinitArrays(Element);
                    Unbind();
                }

                /*
                Must be called before any other buffer operation.
                Pass data == NULL only to reserve space.
                */
                void Initialize(uint elementCount, BufferType type, BufferAccess access = BUFFER_ACCESS_DRAW, const void* data = NULL)
                {
                    ASSERT(Element.Size != 0);

                    GLenum usage = (int)type + (int)access;
                    Bind();
                    glBufferDataARB(target, elementCount * Element.Size, data, usage);
                    _size = elementCount;
                }

                /*
                Copies data into buffer starting from offset.
                Buffer must be bound.
                */
                void Copy(const void* data, uint elementCount, int offsetInElems = 0)
                {
                    ASSERT(Element.Size != 0);
                    glBufferSubDataARB(target, offsetInElems*Element.Size, elementCount*Element.Size, data);
                }

                /*
                Maps buffer onto client memory.
                */
                void* Lock(bool write = true, bool read = false)
                {
                    ASSERT(read || write);

                    GLenum access;
                    if (read && write) access = GL_READ_WRITE;
                    if (read && !write) access = GL_READ_ONLY;
                    if (!read && write) access = GL_WRITE_ONLY;

                    Bind();
                    return glMapBufferARB(target, access);
                }

                /*
                Unmaps the buffer.
                */
                void Unlock()
                {
                    glUnmapBufferARB(target);
                }

                uint GetElementsCount() const { return _size; }

            public:
                VertexDescription Element;

            protected:
                GLuint _id;
                uint _size;
            };
        }

        /*
        Class used to safely (exception-safe) buffer Lock\Unlock.
        */
        template<class BufferType, class VertexType>
        class BufferUpdater
        {
        public:
            BufferUpdater(BufferType* buffer, bool write = true, bool read = false) 
                : _buffer(buffer)
            {
                ASSERT(sizeof(VertexType) == buffer->Element.Size);

                _pointer = (VertexType*)buffer->Lock(write, read);
                _index = 0;
            }
            ~BufferUpdater()
            {
                _buffer->Unlock();
            }

            VertexType* Pointer() { return _pointer; }
            VertexType& operator[](unsigned int i) { return _pointer[i]; }

            void Reset(int index = 0) { _index = index; }
            VertexType& operator()() { return _pointer[_index]; }

            bool Next()
            {
                if (_index+1 >= _buffer->GetElementsCount()) 
                {
                    _index = _buffer->GetElementsCount();
                    return false;
                }
                _index++;
                return true;
            }

            bool Set(const VertexType& t) { _pointer[_index] = t; return Next(); }

            bool CanSet() const { return _index < _buffer->GetElementsCount(); }
            uint GetWrittenCount() const { return _index; }

        private:
            SmartPointer<BufferType> _buffer;
            VertexType* _pointer;
            uint _index;
        };

        // Buffer classes itself
        typedef Implementation::OpenGLBuffer<GL_ARRAY_BUFFER> VertexBuffer;
        typedef Implementation::OpenGLBuffer<GL_ELEMENT_ARRAY_BUFFER> IndexBuffer;
        typedef Implementation::OpenGLBuffer<GL_PIXEL_PACK_BUFFER> PixelPackBuffer;
        typedef Implementation::OpenGLBuffer<GL_PIXEL_UNPACK_BUFFER> PixelUnpackBuffer;
    }
}