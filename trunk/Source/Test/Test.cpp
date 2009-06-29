// Test.cpp : Defines the entry point for the console application.
//

#include "Includes.h"
#include "Common/Logger.h"
#include "Common/Thread.h"
#include "Common/ThreadLocal.h"
#include "Common/StdCommands.h"
#include "Common/Atomic.h"
#include "Common/ObjectPool.h"
#include "Common/Command.h"
#include "Common/Lazy.h"

using namespace P3D;
using namespace P3D::Graphics;
using namespace P3D::World;

class OpenGLTest : 
    public RenderWindow
{
    TimeCounter fps;
    uint frames;
    float _fps;

    P3D::World::World world;
    FlyingCamera* cam;
    bool _boost;
    bool _firstMove;

    Terrain* ter;
    SmartPointer<Texture> tex;
public:

    OpenGLTest() :
      world(Physics::CreatePhysicalWorld())
    { }

    virtual void OnInitialize()
    {
        Graphics::RenderWindow::OnInitialize();

        const char* ext = (const char*)glGetString(GL_EXTENSIONS);

        SetCursorVisible(false);
        SetCursorPosition(400, 300);
        _firstMove = true;

        {
            cam = new FlyingCamera(&world);
            QTransform tr;
            tr.SetIdentity();
            tr.Translation.Set(5, 0, 2500);
            cam->SetTransform(tr);
            cam->SetFarPlane(10000.0f);
            world.AddEntity(cam);
        }

        {
            P3D::World::Box* ground = new P3D::World::Box(&world, 200, 200, 0.5);
            QTransform tr;
            tr.SetIdentity();
            tr.Translation.Set(0, 0, 2400);
            ground->SetTransform(tr);
            world.AddEntity(ground);
        }

        P3D::World::CompoundEntity* compound = new P3D::World::CompoundEntity(&world);
        QTransform tr;
        tr.SetIdentity();
        tr.Translation.Set(0, 0, 2500.0f);
        tr.Rotation.SetEulerAngles(1.0, 2.0, 1.0);
        compound->SetTransform(tr);

        for (int i = 0; i < 10; i++)
        {
            P3D::World::Box* box = new P3D::World::Box(&world, 0.5f, 0.5f, 0.5f);
            box->SetMass(1.0);
            QTransform tr;
            tr.SetIdentity();
            tr.Translation.Set(0, 0, 1.0f * i + 2.0f);
            tr.Rotation.SetAxisAngle(Vector::GetRandomVector(), 1.0);
            box->SetTransform(tr);
            compound->AddEntity(box);
        }
        world.AddEntity(compound);

        tex.Attach(GetTextureManager()->LoadTexture(L"SceneTexture.jpg"));

        ter = new Terrain(&world);
        ter->Load(L"Scene2.raw", 512, 512, 16.0f, 16.0f);
        //ter->Load(L"Plain.raw", 256, 256, 4.0f, 4.0f);
        world.AddEntity(ter);

        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_LIGHTING);
        glEnable(GL_AUTO_NORMAL);
        glEnable(GL_LIGHT0);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        fps.Reset();
        _fps = 0;

        Graphics::InitializeText();
    }

    void CalculateFPS()
    {
        static float time = 0;
        if (fps.GetTotalTime() > 250)
        {
            time += (float)fps.GetTotalTime();
            _fps = (float)frames*4;
            frames = 0;
            fps.Reset();
        }
        frames++;
    }

    virtual void OnDeinitialize()
    {
        Graphics::RenderWindow::OnDeinitialize();
    }

    virtual void OnRender()
    {
        fps.Tick();
        double tick = fps.GetLastTick() / 1000.0;
        CalculateFPS();

        world.Update();

        glEnable(GL_TEXTURE_2D);
        tex->Bind();
        world.Render(cam);
        tex->Unbind();
        glDisable(GL_TEXTURE_2D);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, 800, 600, 0, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 1.0f);
        {
            if (_fps > 10000) _fps = 0;
            std::ostringstream str;
            str << "FPS: " << _fps;
            OutputText(10, 20, 0, str.str().c_str());
        }

        {
            std::ostringstream str;
            str << "Entities: " << g_EntitiesRendered;
            OutputText(10, 40, 0, str.str().c_str());
        }

        {
            std::ostringstream str;
            str << "Polygons: " << g_PolygonCounter;
            OutputText(10, 60, 0, str.str().c_str());
        }
        glPopAttrib();
    }

    virtual void OnKeyDown(SDL_keysym key)
    {
        if (key.sym == SDLK_ESCAPE) 
        {
            Close();
            return;
        }
        if (key.sym == SDLK_w)
        {
            cam->Forward(true);
            return;
        }
        if (key.sym == SDLK_s)
        {
            cam->Backward(true);
            return;
        }
        if (key.sym == SDLK_a)
        {
            cam->Left(true);
            return;
        }
        if (key.sym == SDLK_d)
        {
            cam->Right(true);
            return;
        }
        if (key.sym == SDLK_q)
        {
            cam->Up(true);
            return;
        }
        if (key.sym == SDLK_e)
        {
            cam->Down(true);
            return;
        }
        if (key.sym == SDLK_LSHIFT)
        {
            _boost = !_boost;
            cam->Boost(_boost);
            return;
        }
    }

    virtual void OnKeyUp(SDL_keysym key)
    {
        if (key.sym == SDLK_w)
        {
            cam->Forward(false);
            return;
        }
        if (key.sym == SDLK_s)
        {
            cam->Backward(false);
            return;
        }
        if (key.sym == SDLK_a)
        {
            cam->Left(false);
            return;
        }
        if (key.sym == SDLK_d)
        {
            cam->Right(false);
            return;
        }
        if (key.sym == SDLK_q)
        {
            cam->Up(false);
            return;
        }
        if (key.sym == SDLK_e)
        {
            cam->Down(false);
            return;
        }
    }

    virtual void OnMouseMove(int dx, int dy, int mx, int my)
    {
        if (_firstMove)
        {
            _firstMove = false;
            return;
        }
        dx = mx - 400;
        dy = my - 300;
        if (dx != 0 || dy != 0)
        {
            Logger logger(L"MouseMove");
            SetCursorPosition(400, 300);
            cam->Turn(dx / 350.0f, - dy / 350.0f);
        }
    }
};

int P2DMain()
{
    return OpenGLTest().Run();
}
