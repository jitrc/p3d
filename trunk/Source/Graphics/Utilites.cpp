#include "Includes.h"
#include "Utilites.h"

#include "SDL/include/SDL_syswm.h"

namespace P3D
{
    namespace Graphics
    {
        void InitializeText()
        {
#ifdef P3D_WINDOWS
            SDL_SysWMinfo info;
            SDL_VERSION(&info.version);
            if (SDL_GetWMInfo(&info) < 0)
                return;

            HDC dc = GetDC(info.window);
            HFONT font;
            LOGFONT lf;
            ZeroMemory(&lf, sizeof(lf)); 
            wcscpy_s(lf.lfFaceName, LF_FACESIZE, L"Consolas");
            font = CreateFontIndirect(&lf); 
            SelectObject(dc, font);
            wglUseFontBitmaps(dc, 0, 255, 1000); 
#endif
        }

        void OutputText(float x, float y, float z, const char* format, ...)
        {
            va_list list;
            va_start (list, format);
            char buffer[512];
            vsprintf_s (buffer, 512, format, list);
            va_end (list);
            glRasterPos3f(x, y, z);
            glListBase (1000); 
            glCallLists(strlen(buffer), GL_UNSIGNED_BYTE, buffer); 
        }

        void DrawFrame()
        {
            glPushAttrib(GL_ENABLE_BIT);
            glDisable(GL_LIGHTING);

            glColor3f(1, 1, 1);
            glPointSize(3);
            glBegin(GL_POINTS);
            glVertex3f(0, 0, 0);
            glEnd();

            glPointSize(1);

            glBegin(GL_LINES);
            glColor3f(1, 0, 0);
            glVertex3f(0, 0, 0);
            glVertex3f(1, 0, 0);

            glColor3f(0, 1, 0);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 1, 0);

            glColor3f(0, 0, 1);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 0, 1);
            glEnd();

            glPopAttrib();
        }

        void DrawFrustum(const Frustum& f)
        {
            glPushAttrib(GL_ENABLE_BIT);
            glDisable(GL_LIGHTING);

            Plane farPlane = f(Frustum::FAR_PLANE);
            Plane nearPlane = f(Frustum::NEAR_PLANE);

            Vector nld, nlu, nrd, nru;
            Vector fld, flu, frd, fru;
            bool res = true;

            res &= Plane::GetIntersection(nld, nearPlane, f(Frustum::LEFT_PLANE), f(Frustum::DOWN_PLANE));
            res &= Plane::GetIntersection(nlu, nearPlane, f(Frustum::LEFT_PLANE), f(Frustum::UP_PLANE));
            res &= Plane::GetIntersection(nrd, nearPlane, f(Frustum::RIGHT_PLANE), f(Frustum::DOWN_PLANE));
            res &= Plane::GetIntersection(nru, nearPlane, f(Frustum::RIGHT_PLANE), f(Frustum::UP_PLANE));

            res &= Plane::GetIntersection(fld, farPlane, f(Frustum::LEFT_PLANE), f(Frustum::DOWN_PLANE));
            res &= Plane::GetIntersection(flu, farPlane, f(Frustum::LEFT_PLANE), f(Frustum::UP_PLANE));
            res &= Plane::GetIntersection(frd, farPlane, f(Frustum::RIGHT_PLANE), f(Frustum::DOWN_PLANE));
            res &= Plane::GetIntersection(fru, farPlane, f(Frustum::RIGHT_PLANE), f(Frustum::UP_PLANE));

            if (!res) 
            {
                glPopAttrib();
                return;
            }

            glBegin(GL_LINES);
            glVertex3fv(nld.GetRealPointer());
            glVertex3fv(nlu.GetRealPointer());
            glVertex3fv(nlu.GetRealPointer());
            glVertex3fv(nru.GetRealPointer());
            glVertex3fv(nru.GetRealPointer());
            glVertex3fv(nrd.GetRealPointer());
            glVertex3fv(nrd.GetRealPointer());
            glVertex3fv(nld.GetRealPointer());

            glVertex3fv(fld.GetRealPointer());
            glVertex3fv(flu.GetRealPointer());
            glVertex3fv(flu.GetRealPointer());
            glVertex3fv(fru.GetRealPointer());
            glVertex3fv(fru.GetRealPointer());
            glVertex3fv(frd.GetRealPointer());
            glVertex3fv(frd.GetRealPointer());
            glVertex3fv(fld.GetRealPointer());

            glVertex3fv(nld.GetRealPointer());
            glVertex3fv(fld.GetRealPointer());
            glVertex3fv(nlu.GetRealPointer());
            glVertex3fv(flu.GetRealPointer());
            glVertex3fv(nrd.GetRealPointer());
            glVertex3fv(frd.GetRealPointer());
            glVertex3fv(nru.GetRealPointer());
            glVertex3fv(fru.GetRealPointer());
            glEnd();

            glPopAttrib();
        }

        void DrawSphere(const Sphere& sphere)
        {
            static GLUquadricObj* obj = NULL;
            if (obj == NULL) 
            {
                obj = gluNewQuadric();
                gluQuadricDrawStyle(obj, GLU_LINE);
            }
                        
            glPushMatrix();
            glTranslatef(sphere.GetCenter().x, sphere.GetCenter().y, sphere.GetCenter().z);
            glPushAttrib(GL_CURRENT_BIT);
            DrawFrame();
            glPopAttrib();
            gluSphere(obj, sphere.GetRadius(), 32, 32);
            glPopMatrix();
        }

        void DrawAABB(const AABB& box)
        {
            glPushAttrib(GL_ENABLE_BIT);
            glDisable(GL_LIGHTING);

            glBegin(GL_LINE_LOOP);
            glVertex3f(box.Min().x, box.Min().y, box.Min().z);
            glVertex3f(box.Min().x, box.Min().y, box.Max().z);
            glVertex3f(box.Min().x, box.Max().y, box.Max().z);
            glVertex3f(box.Min().x, box.Max().y, box.Min().z);
            glEnd();

            glBegin(GL_LINE_LOOP);
            glVertex3f(box.Max().x, box.Min().y, box.Min().z);
            glVertex3f(box.Max().x, box.Min().y, box.Max().z);
            glVertex3f(box.Max().x, box.Max().y, box.Max().z);
            glVertex3f(box.Max().x, box.Max().y, box.Min().z);
            glEnd();

            glBegin(GL_LINES);
            glVertex3f(box.Min().x, box.Min().y, box.Min().z);
            glVertex3f(box.Max().x, box.Min().y, box.Min().z);
            glVertex3f(box.Min().x, box.Min().y, box.Max().z);
            glVertex3f(box.Max().x, box.Min().y, box.Max().z);
            glVertex3f(box.Min().x, box.Max().y, box.Max().z);
            glVertex3f(box.Max().x, box.Max().y, box.Max().z);
            glVertex3f(box.Min().x, box.Max().y, box.Min().z);
            glVertex3f(box.Max().x, box.Max().y, box.Min().z);
            glEnd();

            glPopAttrib();
        }
    }
}