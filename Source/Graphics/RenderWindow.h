#pragma once

#include "Common/Thread.h"
#include "Texture.h"

namespace P3D
{
    namespace Graphics
    {
        /*
        OpenGL window.
        Should be only one instance (asserted in constructor).
        */
        class RenderWindow :
            public MainThreadListener
        {
            static RenderWindow* _instance;

        protected:
            RenderWindow();
            virtual ~RenderWindow();

            void SetCursorVisible(bool visible);
            void SetCursorPosition(int x, int y);

            void SetWindowCaption(const char* caption);
            void Close();

            virtual void OnInitialize();
            virtual void OnDeinitialize();
            virtual void OnRender() {}
            virtual void OnKeyDown(SDL_keysym key) {}
            virtual void OnKeyUp(SDL_keysym key) {}
            virtual void OnMouseDown(uint buttonIndex, int mx, int my) {}
            virtual void OnMouseUp(uint buttonIndex, int mx, int my) {}
            virtual void OnMouseMove(int dx, int dy, int mx, int my) {}

            /*
            Return texture manager. Available only after OnInitialize call.
            */
            TextureManager* GetTextureManager() { return _textureManager; }

        public:
            static RenderWindow* GetInstance() { return _instance; }

            /*
            Blocking call.
            Start rendering loop.
            */
            int Run(uint mainLoopDelay = 0);

        protected:
            // From MainThreadListener
            override void OnIdle();
            override void OnResize(int width, int height);

            Logger logger;

        private:
            TextureManager* _textureManager;
        };
    }
}