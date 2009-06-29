#pragma once

namespace P3D
{
    namespace Graphics
    {
        /*
        Possible fields in the vertex.
        */
        enum VertexDescriptionField
        {
            VD_COORD = 0,
            VD_NORMAL,
            VD_COLOR,
            VD_COLOR2,
            VD_FOG_COORD,
            VD_TEXTURE0,
            VD_TEXTURE1,
            VD_TEXTURE2,
            VD_TEXTURE3,
            VD_TEXTURE4,
            VD_TEXTURE5,
            VD_TEXTURE6,
            VD_TEXTURE7,

            VD_VERTEX_ATTRIB0,
            VD_VERTEX_ATTRIB1,
            VD_VERTEX_ATTRIB2,
            VD_VERTEX_ATTRIB3,
            VD_VERTEX_ATTRIB4,
            VD_VERTEX_ATTRIB5,
            VD_VERTEX_ATTRIB6,
            VD_VERTEX_ATTRIB7
        };

        /*
        Description of the one field.
        */
        struct VertexFieldDescription
        {
            VertexFieldDescription(int offset = -1, GLenum dataType = 0, GLint size = 0, bool normalized = false)
            {
                this->offset = offset;
                this->size = size;
                this->type = dataType;
                this->normalized = normalized;
            }

            int offset; // offset in the struct
            GLint size; // number of components
            GLenum type; // type of the each component
            bool normalized; // used with VD_VERTEX_ATTRIB#
        };

        /*
        Describe layout of vertex data in the VB.
        */
        class VertexDescription
        {
        public:
            VertexDescription()
            {
                Size = 0;
            }

            /*
            Mark field in the struct. Type and count are deduced from template instantation.
            */
            template<class T, class M, int n>
            void Add(VertexDescriptionField type, M (T::* ptr)[n])
            {
                Size = sizeof(T);
                RegisterField(type, offsetof(T, *ptr), GetDataType<M>::Value, n);
            }

            template<class T, class M>
            void Add(VertexDescriptionField type, M T::* ptr)
            {
                Size = sizeof(T);
                RegisterField(type, offsetof(T, *ptr), GetDataType<M>::Value, 1);
            }


            template<class T, class M, int n>
            void AddAttrib(int index, M (T::* ptr)[n], bool normalized = false)
            {
                Size = sizeof(T);
                RegisterField((VertexDescriptionField)(VD_VERTEX_ATTRIB0 + index), offsetof(T, *ptr), GetDataType<M>::Value, n, normalized);
            }

            template<class T, class M>
            void AddAttrib(int index, M T::* ptr, bool normalized = false)
            {
                Size = sizeof(T);
                RegisterField((VertexDescriptionField)(VD_VERTEX_ATTRIB0 + index), offsetof(T, *ptr), GetDataType<M>::Value, 1, normalized);
            }

            void Clear()
            {
                Size = 0;
                Fields.clear();
            }

        public:
            typedef std::hash_map<VertexDescriptionField, VertexFieldDescription>::const_iterator FieldIterator;

            uint Size;
            std::hash_map<VertexDescriptionField, VertexFieldDescription> Fields;

        private:
            template<class M>
            struct GetDataType
            {
                static const GLenum Value = ::Type_Is_Not_Supported;
            };

            #define DEFINE_TYPE_ENUM(type, enumValue) \
                template<> \
                        struct GetDataType<type> \
                        { \
                        static const GLenum Value = enumValue; \
                        };

            DEFINE_TYPE_ENUM(short, GL_SHORT);
            DEFINE_TYPE_ENUM(ushort, GL_UNSIGNED_SHORT);
            DEFINE_TYPE_ENUM(int, GL_INT);
            DEFINE_TYPE_ENUM(uint, GL_UNSIGNED_INT);
            DEFINE_TYPE_ENUM(byte, GL_UNSIGNED_BYTE);
            DEFINE_TYPE_ENUM(char, GL_BYTE);
            DEFINE_TYPE_ENUM(float, GL_FLOAT);
            DEFINE_TYPE_ENUM(double, GL_DOUBLE);

            #undef DEFINE_TYPE_ENUM

            void RegisterField(VertexDescriptionField type, int offset, GLenum dataType, GLint size, bool normalized = false)
            {
                Fields[type] = VertexFieldDescription(offset, dataType, size, normalized);
            }
        };
    }
}