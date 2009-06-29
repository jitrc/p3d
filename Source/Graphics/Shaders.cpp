#include "Includes.h"
#include "Shaders.h"

namespace P3D
{
    namespace Graphics
    {
        Shader::Shader(ShaderType type)
        {
            _compiled = false;
            _id = 0;
            GLenum _type;
            switch (type)
            {
                case VERTEX_SHADER: _type = GL_VERTEX_SHADER_ARB; break;
                case PIXEL_SHADER: _type = GL_FRAGMENT_SHADER_ARB; break;
                default:
                    ASSERT(false);
            }
            _id = glCreateShaderObjectARB(_type);
        }

        Shader::~Shader()
        {
            if (_id) glDeleteObjectARB(_id);
        }

        bool Shader::LoadSource(const char* source)
        {
            if (_id) 
            {
                const char* sources[] = { source };
                glShaderSourceARB(_id, 1, sources, NULL);
                _compiled = false;
                _log = "";
            }
            return Compile();
        }

        bool Shader::LoadSource(const char** sources, int count)
        {
            if (_id)
            {
                glShaderSourceARB(_id, count, sources, NULL);
                _compiled = false;
                _log = "";
            }
            return Compile();
        }

        bool Shader::Compile()
        {
            if (_id)
            {
                glCompileShaderARB(_id);
                GLint compiled = GL_FALSE;
                glGetObjectParameterivARB(_id, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);

                char buffer[2048];
                int len = 2048;
                buffer[0] = 0;
                glGetInfoLogARB(_id, 2048, &len, buffer);
                _log = buffer;
                return (_compiled = (compiled == GL_TRUE));
            } else
            {
                _log = "Shaders are not supported";
                return false;
            }
        }

        ///////////////////////

        ShaderProgram::ShaderProgram()
        {
            _linked = false;
            _id = glCreateProgramObjectARB();
        }

        ShaderProgram::~ShaderProgram()
        {
            if (_id) glDeleteObjectARB(_id);
            for (uint i = 0; i < _shaders.size(); i++)
                _shaders[i]->Release();
        }

        void ShaderProgram::Attach(Shader* s)
        {
            ASSERT(s->IsCompiled());
            s->AddRef();
            _shaders.push_back(s);
            if (_id) glAttachObjectARB(_id, s->_id);
            _linked = false;
        }

        void ShaderProgram::Detach(Shader* s)
        {
            //for (uint i = 0; i < _shaders.size(); i++)
            //{
            //    if (_shaders[i] == s)
            //    {
            //        if (_id) glDetachShader(_id, s->_id); // ???
            //        s->Release();
            //        _shaders.erase(_shaders.begin() + i);
            //        _linked = false;
            //        return;
            //    }
            //}
        }

        bool ShaderProgram::Link()
        {
            if (_id)
            {
                glLinkProgramARB(_id);
                GLint linked = GL_FALSE;
                glGetObjectParameterivARB(_id, GL_OBJECT_LINK_STATUS_ARB, &linked);

                char buffer[2048];
                int len = 2048;
                buffer[0] = 0;
                glGetInfoLogARB(_id, 2048, &len, buffer);
                _log = buffer;
                return (_linked = (linked == GL_TRUE));
            } else
            {
                _log = "Shaders are not supported";
                return false;
            }
        }

        void ShaderProgram::BindVertexAttribute(int index, const char* name)
        {
            if (_id) glBindAttribLocationARB(_id, index, name);
        }

        bool ShaderProgram::SetVariable(const char* name, int varDim, const float* values, int count)
        {
            if (!_linked) return false;
            if (_id)
            {
                GLint location = glGetUniformLocationARB(_id, name);
                if (location == -1) return false;
                switch (varDim)
                {
                    case 1: glUniform1fvARB(location, count, values); break;
                    case 2: glUniform2fvARB(location, count, values); break;
                    case 3: glUniform3fvARB(location, count, values); break;
                    case 4: glUniform4fvARB(location, count, values); break;
                    default:
                        return false;
                }
                return true;
            } else
                return false;
        }

        bool ShaderProgram::SetVariable(const char* name, int varDim, const int* values, int count)
        {
            if (!_linked) return false;
            if (_id)
            {
                GLint location = glGetUniformLocationARB(_id, name);
                if (location == -1) return false;
                switch (varDim)
                {
                    case 1: glUniform1ivARB(location, count, values); break;
                    case 2: glUniform2ivARB(location, count, values); break;
                    case 3: glUniform3ivARB(location, count, values); break;
                    case 4: glUniform4ivARB(location, count, values); break;
                    default:
                        return false;
                }
                return true;
            } else
                return false;
        }

        bool ShaderProgram::SetMatrixVariable(const char* name, int dim, const float* values, int count, bool transpose)
        {
            if (!_linked) return false;
            if (_id)
            {
                GLint location = glGetUniformLocationARB(_id, name);
                if (location == -1) return false;
                GLboolean trans = transpose ? GL_TRUE : GL_FALSE;
                switch (dim)
                {
                    case 2: glUniformMatrix2fvARB(location, count, trans, values); break;
                    case 3: glUniformMatrix3fvARB(location, count, trans, values); break;
                    case 4: glUniformMatrix4fvARB(location, count, trans, values); break;
                    default:
                        return false;
                }
                return true;
            } else
                return false;
        }


        void ShaderProgram::Activate()
        {
            if (_linked) glUseProgramObjectARB(_id);
        }

        void ShaderProgram::Deactivate()
        {
            glUseProgramObjectARB(0);
        }
    }
}