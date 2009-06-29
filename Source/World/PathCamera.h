#pragma once

#include "Camera.h"

namespace P3D
{
    namespace World
    {
        /*
        Camera that moves along the predefined path.
        */
        class PathCamera : public Camera
        {
        public:
            typedef Path<QTransform> PathType;

        public:
            PathCamera(World* world);

            /*
            Set active camera path.
            Reset time as well.
            */
            inline void SetCameraPath(PathType* path) 
            { 
                _path = path; 
                TimeAlongPath = 0;
            }

            /*
            Get the path the camera is on now.
            */
            inline const PathType* GetCameraPath() const { return _path; }

            /*
            Moves the camera along the path.
            */
            override bool Update();

        public:
            /*
            Transformation of the path.
            */
            QTransform PathTransform;

            /*
            Time scale. 1.0 is default. If <1 move slower, >1 move faster.
            */
            Scalar TimeScale;

            /*
            Parameter of the path curve.
            */
            Scalar TimeAlongPath;

            /*
            Loop the path?
            */
            bool Loop;

        private:
            SmartPointer<PathType> _path; 
        };
    }
}