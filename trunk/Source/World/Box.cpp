#include "Includes.h"
#include "Box.h"
#include "World.h"

namespace P3D
{
    namespace World
    {
        Box::Box(World* world, Scalar dx, Scalar dy, Scalar dz)
            : Entity(world), _dx(dx / 2), _dy(dy / 2), _dz(dz / 2)
        {
            if (world->GetPhysicalWorld())
            {
                Physics::CollisionModel* cm = world->GetPhysicalWorld()->CreateBoxCollisionModel(dx, dy, dz);
                SetCollisionModel(cm);
                cm->Release();
            }
        }

        void Box::RecalculateBoundingBox(AABB& box)
        {
            box.Min().Set(- _dx, - _dy,  - _dz);
            box.Max().Set(  _dx,   _dy,    _dz);
        }

        void Box::DoRender(const RendererContext& context)
        {
            uint32 color = GetColor();
            glColor3ubv((GLubyte*)&color);

            GLubyte* ubColor = (GLubyte*)&color;
            GLfloat fcolor[] = {ubColor[0] / 255.0f, ubColor[1] / 255.0f, ubColor[2] / 255.0f, 1.0f};
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, fcolor);

            glBegin(GL_QUADS);
            glNormal3f(0, 0, 1);
            glVertex3f(-_dx, -_dy,  _dz);
            glVertex3f( _dx, -_dy,  _dz);
            glVertex3f( _dx,  _dy,  _dz);
            glVertex3f(-_dx,  _dy,  _dz);

            glNormal3f(0, 0, -1);
            glVertex3f( _dx, -_dy, -_dz);
            glVertex3f(-_dx, -_dy, -_dz);
            glVertex3f(-_dx,  _dy, -_dz);
            glVertex3f( _dx,  _dy, -_dz);

            glNormal3f(-1, 0, 0);
            glVertex3f(-_dx, -_dy, -_dz);
            glVertex3f(-_dx, -_dy,  _dz);
            glVertex3f(-_dx,  _dy,  _dz);
            glVertex3f(-_dx,  _dy, -_dz);

            glNormal3f(1, 0, 0);
            glVertex3f( _dx, -_dy,  _dz);
            glVertex3f( _dx, -_dy, -_dz);
            glVertex3f( _dx,  _dy, -_dz);
            glVertex3f( _dx,  _dy,  _dz);

            glNormal3f(0, -1, 0);
            glVertex3f(-_dx, -_dy,  _dz);
            glVertex3f(-_dx, -_dy, -_dz);
            glVertex3f( _dx, -_dy, -_dz);
            glVertex3f( _dx, -_dy,  _dz);

            glNormal3f(0, 1, 0);
            glVertex3f(-_dx,  _dy, -_dz);
            glVertex3f(-_dx,  _dy,  _dz);
            glVertex3f( _dx,  _dy,  _dz);
            glVertex3f( _dx,  _dy, -_dz);
            glEnd();

            Entity::DoRender(context);
        }
    }
}