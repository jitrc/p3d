#include "Includes.h"
#include "VertexBuffer.h"

namespace P3D
{
    namespace Graphics
    {
        namespace Implementation
        {
            static inline void SetupPointer(const VertexDescriptionField type, int stride, const VertexFieldDescription& vd)
            {
                switch (type)
                {
                case VD_COORD:
                    {
                        glEnableClientState(GL_VERTEX_ARRAY);
                        glVertexPointer(vd.size,  vd.type, stride, (GLvoid*)vd.offset);
                    }
                    break;
                case VD_NORMAL:
                    {
                        glEnableClientState(GL_NORMAL_ARRAY);
                        glNormalPointer(vd.type, stride, (GLvoid*)vd.offset);
                    }
                    break;
                case VD_COLOR:
                    {
                        glEnableClientState(GL_COLOR_ARRAY);
                        glColorPointer(vd.size,  vd.type, stride, (GLvoid*)vd.offset);
                    }
                    break;
                case VD_COLOR2:
                    {
                        glEnableClientState(GL_SECONDARY_COLOR_ARRAY);
                        glSecondaryColorPointer(vd.size,  vd.type, stride, (GLvoid*)vd.offset);
                    }
                    break;
                case VD_FOG_COORD:
                    {
                        glEnableClientState(GL_FOG_COORD_ARRAY);
                        glFogCoordPointer(vd.type, stride, (GLvoid*)vd.offset);
                    }
                    break;
                }			

                if (type >= VD_TEXTURE0 && type <= VD_TEXTURE7)
                {
                    int index = (int)type - (int)VD_TEXTURE0;
                    glActiveTexture(GL_TEXTURE0 + index);
                    glClientActiveTexture(GL_TEXTURE0 + index);
                    glEnableClientState (GL_TEXTURE_COORD_ARRAY); 
                    glTexCoordPointer(vd.size, vd.type, stride, (GLvoid*)vd.offset);
                }

                if (type >= VD_VERTEX_ATTRIB0 && type <= VD_VERTEX_ATTRIB7)
                {
                    int index = (int)type - (int)VD_VERTEX_ATTRIB0;
                    glEnableVertexAttribArrayARB(index);
                    glVertexAttribPointerARB(index, vd.size, vd.type, vd.normalized, stride, (GLvoid*)vd.offset);
                }
            }

            static inline void ShutdownPointer(const VertexDescriptionField type)
            {
                switch (type)
                {
                case VD_COORD:
                    {
                        glDisableClientState(GL_VERTEX_ARRAY);
                    }
                    break;
                case VD_NORMAL:
                    {
                        glDisableClientState(GL_NORMAL_ARRAY);
                    }
                    break;
                case VD_COLOR:
                    {
                        glDisableClientState(GL_COLOR_ARRAY);
                    }
                    break;
                case VD_COLOR2:
                    {
                        glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
                    }
                    break;
                case VD_FOG_COORD:
                    {
                        glDisableClientState(GL_FOG_COORD_ARRAY);
                    }
                    break;
                }			

                if (type >= VD_TEXTURE0 && type <= VD_TEXTURE7)
                {
                    int index = (int)type - (int)VD_TEXTURE0;
                    glActiveTexture(GL_TEXTURE0 + index);
                    glClientActiveTexture(GL_TEXTURE0 + index);
                    glDisableClientState(GL_TEXTURE_COORD_ARRAY); 
                }

                if (type >= VD_VERTEX_ATTRIB0 && type <= VD_VERTEX_ATTRIB7)
                {
                    int index = (int)type - (int)VD_VERTEX_ATTRIB0;
                    glDisableVertexAttribArrayARB(index);
                }
            }


            void InitArrays(const VertexDescription& vd)
            {
                // init pointers
                for (VertexDescription::FieldIterator field = vd.Fields.begin(); 
                    field != vd.Fields.end(); ++field)
                {
                    if (field->second.offset != -1)
                        SetupPointer(field->first, vd.Size, field->second);
                }
            }

            void DeinitArrays(const VertexDescription& vd)
            {
                // Shutdown pointers
                for (VertexDescription::FieldIterator field = vd.Fields.begin(); 
                    field != vd.Fields.end(); ++field)
                {
                    if (field->second.offset != -1)
                        ShutdownPointer(field->first);
                }
            }
        }
    }
}