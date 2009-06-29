#include "Includes.h"
#include "PathCamera.h"
#include "World.h"

namespace P3D
{
    namespace World
    {
        PathCamera::PathCamera(World* world)
            : Camera(world)
        {
            TimeScale = 1.0f;
            TimeAlongPath = 0.0f;
            Loop = false;

            PathTransform.SetIdentity();
        }

        bool PathCamera::Update()
        {
            if (!NeedUpdate()) return false;
            if (_path)
            {
                TimeAlongPath += TimeScale * GetWorld()->Time().GetLastTick() / 1000.0f;

                // loop
                if (TimeAlongPath > _path->MaxT)
                {
                    if (Loop)
                        TimeAlongPath = _path->MinT + (TimeAlongPath - _path->MaxT);
                    else
                    {
                        SetCameraPath(NULL);
                        return Camera::Update();
                    }
                }

                QTransform tr;
                _path->Interpolate(TimeAlongPath, tr);
                tr.Rotation.Normalize();
                tr = tr * PathTransform;
                SetTransform(tr);
            }
            return Camera::Update();
        }
    }
}