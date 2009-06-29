#pragma once

#include "Camera.h"

namespace P3D
{
    namespace World
    {
        class FlyingCamera : public Camera
        {
        public:
            FlyingCamera(World* world);

            void Forward(bool onOff) { _forward = onOff; }
            void Backward(bool onOff) { _backward = onOff; }
            void Left(bool onOff) { _left = onOff; }
            void Right(bool onOff) { _right = onOff; }
            void Up(bool onOff) { _up = onOff; }
            void Down(bool onOff) { _down = onOff; }
            void Boost(bool onOff) { _boost = onOff; }

            void ResetMovement()
            {
                _forward = _backward = _left = _right = _up = _down = _boost = false;
            }

            void Turn(float dx, float dy);

            void GetRotationAngles(float& x, float& y) const { x = _yaw; y = _pitch; }
            void SetRotationAngles(float x, float y) { _yaw = x; _pitch = y; Turn(0, 0); }

            override bool Update();

        private:
            bool _forward, _backward;
            bool _left, _right;
            bool _up, _down;
            bool _boost;

            float _yaw, _pitch;
        };
    }
}