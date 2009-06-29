#include "Includes.h"
#include "World.h"

#include "Common/Counters.h"

namespace P3D
{
    namespace World
    {
        World::World(Physics::PhysicalWorld* physicalWorld) : CompoundEntity(this)
        {
            _activeCamera = NULL;
            _timeCounter.Reset();
            _physicalWorld.Attach(physicalWorld);

            _worldBounds.Points[0].Set(-1000, -1000, -1000);
            _worldBounds.Points[1].Set( 1000,  1000,  1000);
        }

        World::~World()
        {
            RemoveAllEntities();
        }

        void World::Prepare()
        {
            if (_physicalWorld)
                _physicalWorld->Prepare(this);
            CompoundEntity::Prepare();
        }

        bool World::Update()
        {
            if (_timeCounter.GetTicks() == 0)
            {
                _timeCounter.Reset();
                Prepare();
            }
            _timeCounter.Tick();
            if (_physicalWorld) _physicalWorld->Update(_timeCounter);
            return CompoundEntity::Update();
        }

        void World::Render(Camera* camera)
        {
            ASSERT(camera != NULL);

            // activate camera
            _activeCamera = camera;
            camera->LoadCameraTransform();

            // reset counters ;)
            ClearCounters();

            // get camera frustum in world space
            Frustum frustum = _activeCamera->GetFrustum();
            frustum.Transform(_activeCamera->GetTransformToWorldSpace());

            // render
            RendererContext context(frustum);
            context.Flags = RF_RenderHelpers;// | RF_RenderAll;
            Render(context);

            // deactivate camera
            _activeCamera = NULL;
        }
    }
}