#pragma once

#include "VertexBuffer.h"

namespace P3D
{
    namespace Graphics
    {
        class Texture;
        class TextureManager;

        enum TexelFormat
        {
            TEXEL_RGB = 0,
            TEXEL_RGBA,
            TEXEL_BGR,
            TEXEL_BGRA,
            TEXEL_LUMINANCE
        };

        /*
        Texture object. Incapsulates texture data.
        */
        class Texture : 
            public Object
        {
            friend class TextureManager;

        protected:
            Texture(GLuint id);
            virtual ~Texture();

        public:
            inline void Bind()
            {
                glBindTexture(GL_TEXTURE_2D, _id);
            }

            inline void Unbind()
            {
                glBindTexture(GL_TEXTURE_2D, 0);
            }

        protected:
            GLuint _id;
        };

        /*
        Textures that can be accessed through code.
        */
        class CustomTexture : public Texture
        {
            friend class TextureManager;

        protected:
            CustomTexture(GLuint id, uint width, uint height, TexelFormat format);
            virtual ~CustomTexture() {};

        public:
            /*
            Begin texture modification.
            */
            byte* Lock(uint x, uint y, uint width, uint height);

            /*
            End texture modification.
            */
            void Unlock();

        protected:
            uint _updateX, _updateY, _updateWidth, _updateHeight;
            TexelFormat _format;
            SmartPointer<PixelUnpackBuffer> _pixelBuffer;
        };

        /*
        Load textures.
        */
        class TextureManager
        {
            static Logger logger;

        public:
            /*
            Constrctor.
            */
            TextureManager();

            /*
            Releases all textures.
            */
            virtual ~TextureManager();

            /*
            Load texture from file.
            */
            Texture* LoadTexture(const wchar* file);

            /*
            Create empty custom texture.
            */
            CustomTexture* CreateCustomTexture(uint width, uint height, TexelFormat format);

            /*
            Destroy textures that are not referenced by anyone.
            */
            void UnloadUnusedTextures();

        private:
            Texture _defaultTexture;

            /*
            Map of all active textures.
            */
            typedef std::hash_map<std::wstring, Texture*> TexturesMap;
            TexturesMap _textures;
        };

    }
}