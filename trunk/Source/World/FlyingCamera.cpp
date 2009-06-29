#include "Includes.h"
#include "FlyingCamera.h"
#include "World.h"

namespace P3D
{
    namespace World
    {
        const float ACCEL = 10.0f;
        
        FlyingCamera::FlyingCamera(World* world)
            : Camera(world)
        {
            _forward = false;
            _backward = false;
            _left = false;
            _right = false;
            _up = false;
            _down = false;
            _boost = false;
            _yaw = 0;
            _pitch = 0;
        }

        void FlyingCamera::Turn(float dx, float dy)
        {
            _yaw -= dx;
            _pitch -= dy;
            
            Clamp(_pitch, float(-M_PI/2.0 + 0.001f), float(+M_PI/2.0 - 0.001f));

            Vector x;
            x.SetSphericalCoordinates(_pitch, _yaw, 1.0);
            LookAt(-x);
        }

        bool FlyingCamera::Update()
        {
            if (!NeedUpdate()) return false;
            float forward = 0;
            float left = 0;
            float up = 0;
            if (_forward) forward = 1; else if (_backward) forward = -1;
            if (_left) left = -1; else if (_right) left = 1;
            if (_up) up = 1; else if (_down) up = -1;

            Scalar vel = ACCEL;
            if (_boost) vel *= 30.0f;
            Vector dir(-vel*forward, vel*left, vel*up);
            QTransform tr = GetTransform();
            dir.Transform(tr.Rotation);
            tr.Translation += dir * (GetWorld()->Time().GetLastTick() / 1000.0f);
            SetTransform(tr);
            
            return Camera::Update();
        }
    }
}