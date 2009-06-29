#include "Includes.h"
#include "Texture.h"

namespace P3D
{
    namespace Graphics
    {
        Logger TextureManager::logger(L"Graphics.TextureManager");

        // indices are TexelFormat enums
        static int BytesPerTexel[] =
        {
            3,
            4,
            3,
            4,
            1
        };

        static int OpenGLTexelFomat[] =
        {
            GL_RGB,
            GL_RGBA,
            GL_BGR,
            GL_BGRA,
            GL_LUMINANCE
        };

        TextureManager::TextureManager()
            : _defaultTexture(0)
        {
            // init DevIL
            ilInit(); 
            ilutInit();

            // say that we are using OpenGL
            ilutRenderer(ILUT_OPENGL);

            // OpenGL image origin mode
            ilEnable(IL_ORIGIN_SET);
            ilSetInteger(IL_ORIGIN_MODE, IL_ORIGIN_UPPER_LEFT);

            // keep DDS compressed
            ilSetInteger(IL_KEEP_DXTC_DATA, 1); 

            // do not event decompress it
            ilSetInteger(IL_DO_NOT_DECOMPRESS_DXTC, 1); 
            ilutEnable(ILUT_GL_USE_S3TC);
        }

        TextureManager::~TextureManager()
        {
            // remove all textures
            for (TexturesMap::iterator it = _textures.begin(); it != _textures.end(); ++it)
                it->second->Release();
            _textures.clear();
        }

        /*
        Load texture from file.
        */
        Texture* TextureManager::LoadTexture(const wchar* file)
        {
            logger.info() << L"Loading texture from '" << file << L"'...";

            // search among loaded
            TexturesMap::const_iterator it = _textures.find(file);
            if (it != _textures.end())
            {
                // found!
                logger.info() << L"Already loaded.";
                it->second->AddRef();
                return it->second;
            }

            ILint image = ilGenImage();
            ILboolean result = ilLoad(IL_TYPE_UNKNOWN, file);
            if (!result)
            {
                ilDeleteImage(image);
                logger.error() << L"Error while loading '" << file << L"': " << 
                    ToUTF16((const char*)iluErrorString(ilGetError()));
                _defaultTexture.AddRef();
                return &_defaultTexture;
            } else
            {
                Texture* texture = NULL;
                GLuint id = ilutGLBindTexImage();
                texture = new Texture(id);
                texture->AddRef();
                _textures[file] = texture;
                return texture;
            }
        }

        void TextureManager::UnloadUnusedTextures()
        {
            std::vector<TexturesMap::iterator> toRemove;
            for (TexturesMap::iterator it = _textures.begin(); it != _textures.end(); ++it)
            {
                if (it->second->AddRef() == 2)
                {
                    toRemove.push_back(it);
                    it->second->Release();
                }
                it->second->Release();
            }

            for (std::vector<TexturesMap::iterator>::iterator it = toRemove.begin(); it != toRemove.end(); ++it)
                _textures.erase(*it);
        }

        CustomTexture* TextureManager::CreateCustomTexture(uint width, uint height, TexelFormat format)
        {
            GLuint id = 0;
            glGenTextures(1, &id);
            return new CustomTexture(id, width, height, format);
        }

        Texture::Texture(GLuint id)
        {
            _id = id;
        }

        Texture::~Texture()
        {
            glDeleteTextures(1, &_id);
        }

        CustomTexture::CustomTexture(GLuint id, uint width, uint height, TexelFormat format) : Texture(id) 
        {
            _updateX = 0;
            _updateY = 0;
            _updateWidth = 0;
            _updateHeight = 0;
            _format = format;

            Bind();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, BytesPerTexel[_format], width, height, 
                0, OpenGLTexelFomat[_format], GL_UNSIGNED_BYTE, NULL);
        }

        byte* CustomTexture::Lock(uint x, uint y, uint width, uint height)
        {
            ASSERT(_pixelBuffer == NULL);
            ASSERT(_updateWidth == 0 && _updateHeight == 0);
            ASSERT(width != 0 && height != 0);

            _updateX = x;
            _updateY = y;
            _updateWidth = width;
            _updateHeight = height;

            _pixelBuffer.Attach(new PixelUnpackBuffer());
            _pixelBuffer->Element.Size = BytesPerTexel[_format];
            _pixelBuffer->Initialize(width * height, BUFFER_STATIC);
            return (byte*)_pixelBuffer->Lock();
        }

        void CustomTexture::Unlock()
        {
            ASSERT(_updateWidth != 0 && _updateHeight != 0);
            ASSERT(_pixelBuffer != NULL);

            _pixelBuffer->Unlock();
            Bind();
            glTexSubImage2D(GL_TEXTURE_2D, 0, _updateX, _updateY, _updateWidth, _updateHeight, 
                OpenGLTexelFomat[_format], GL_UNSIGNED_BYTE, 0);
            _pixelBuffer->Unbind();

            _updateX = 0;
            _updateY = 0;
            _updateWidth = 0;
            _updateHeight = 0;
            _pixelBuffer = NULL;
        }
    }
}