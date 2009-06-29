#include "Includes.h"
#include "RenderWindow.h"

namespace P3D
{
    namespace Graphics
    {
        RenderWindow* RenderWindow::_instance = NULL;

        RenderWindow::RenderWindow() : logger(L"Graphics.RenderWindow"),
            _textureManager(NULL)
        {
            ASSERT(_instance == NULL);
            _instance = this;
            Thread::GetMainThread()->AddListener(this);
        }

        RenderWindow::~RenderWindow()
        {
            ASSERT(_instance == this);
            Thread::GetMainThread()->RemoveListener(this);
            _instance = NULL;
        }

        void RenderWindow::SetCursorVisible(bool visible)
        {
            int vis = SDL_ShowCursor(-1);
            if ((vis && !visible) || (!vis && visible))
                SDL_ShowCursor(visible);
        }

        void RenderWindow::SetCursorPosition(int x, int y)
        {
            SDL_WarpMouse(x, y);
        }

        void RenderWindow::SetWindowCaption(const char* caption)
        {
            SDL_WM_SetCaption(caption, "P3D");
        }

        void RenderWindow::Close()
        {
            Thread::GetMainThread()->Stop();
        }

        void RenderWindow::OnInitialize()
        {
            //TODO: add config
            logger.info() << L"Initializing OpenGL...";

            SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
            SDL_SetVideoMode(800, 600, 0, SDL_OPENGL | SDL_ANYFORMAT);
            SDL_WM_SetCaption("P3D RenderWindow", "P3D");

            logger.info() << L"Loading extensions...";

            GLenum err = glewInit();
            if (GLEW_OK != err)
            {
                logger.warn() << L"glewInit something is seriously wrong. Error: " 
                    << ToUTF16((const char*)glewGetErrorString(err));
            }

            logger.info() << L"Creating texture manager...";
            _textureManager = new TextureManager();

            logger.info() << L"OpenGL initialization completed.";
        }

        void RenderWindow::OnDeinitialize()
        {
            if (_textureManager)
            {
                delete _textureManager;
                _textureManager = NULL;
            }
        }

        void RenderWindow::OnIdle()
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            OnRender();
            SDL_GL_SwapBuffers();
        }

        void RenderWindow::OnResize(int width, int height)
        {
            glViewport(0, 0, width, height);
        }

        int RenderWindow::Run(uint mainLoopDelay)
        {
            Thread::GetMainThread()->SetMainLoopDelay(mainLoopDelay);
            Thread::GetMainThread()->Run("MainThread");
            return 0;
        }
    }
}