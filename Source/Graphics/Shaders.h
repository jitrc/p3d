#pragma once

namespace P3D
{
    namespace Graphics
    {
        enum ShaderType
        {
            VERTEX_SHADER,
            PIXEL_SHADER
        };

        /*
        Single shader compilation unit.
        */
        class Shader : public Object
        {
            friend class ShaderProgram;

        public:
            /*
            Create empty shader of specific type.
            */
            Shader(ShaderType type);
            virtual ~Shader();

            /*
            Load and compile shader from source.
            Return true in case of successful compilation.
            Compilation log can be obtained by the call to GetCompilationLog().
            */
            bool LoadSource(const char* source);

            /*
            Same as LoadSource(const char* source); but links several sources together as one.
            */
            bool LoadSource(const char** sources, int count);

            bool IsCompiled() const { return _compiled; }
            const char* GetCompilationLog() const { return _log.c_str(); }

        private:
            GLuint _id;
            bool _compiled;
            std::string _log;

            bool Compile();
        };

        /*
        Several Shader objects linked together.
        */
        class ShaderProgram : public Object
        {
        public:
            ShaderProgram();
            virtual ~ShaderProgram();

            /*
            Attach succesfully compiled Shader.
            */
            void Attach(Shader* s);

            /*
            Detach previously attached Shader.
            */
            void Detach(Shader* s);

            /*
            Binds attribute index with attribute name.
            Must be called before Link()
            */
            void BindVertexAttribute(int index, const char* name);

            /*
            Links all attached Shaders into complete ready to use program.
            Return true in case of success.
            */
            bool Link();

            bool IsLinked() const { return _linked; }
            const char* GetLinkLog() const { return _log.c_str(); }

            /*
            All methods below can only be called after linking.
            */

            /*
            Set the uniform varible name.
            The dimension of the varible must be equal to varDim (for example 3 for vec3).
            The passed values is 'values'. Function sets 'count' number of values in the array. 
            */
            bool SetVariable(const char* name, int varDim, const float* values, int count = 1);
            bool SetVariable(const char* name, int varDim, const int* values, int count = 1);

            bool SetVariable(const char* name, const float value)
            {
                return SetVariable(name, 1, &value);
            }

            bool SetVector3(const char* name, const Vector& value, int count = 1) { return SetVariable(name, 3, value.GetRealPointer(), count); }
            bool SetVector4(const char* name, const Vector& value, int count = 1) { return SetVariable(name, 4, value.GetRealPointer(), count); }

            bool SetMatrixVariable(const char* name, int dim, const float* value, int count = 1, bool transpose = false);

            bool SetMatrix3(const char* name, const Matrix& matrix, int count = 1, bool transpose = false)
            {
                float m[3][3] = 
                {
                    { matrix(0, 1), matrix(0, 2), matrix(0, 3) },
                    { matrix(1, 1), matrix(1, 2), matrix(1, 3) },
                    { matrix(2, 1), matrix(2, 2), matrix(2, 3) },
                };
                SetMatrixVariable(name, 3, &m[0][0], count, transpose);
            }

            /*
            Installs program into pipeline.
            */
            void Activate();

            /*
            Swaps to default pipeline.
            */
            void Deactivate();

        private:
            GLuint _id;
            std::vector<Shader*> _shaders;
            bool _linked;
            std::string _log;
        };
    }
}