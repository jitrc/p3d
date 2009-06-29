#include "Includes.h"
#include "Camera.h"
#include "World.h"

namespace P3D
{
    namespace World
    {
        static GLfloat gSwapAxisMatrix[16] =
        {
            0, 0, 1, 0,
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 0, 1
        };

        Camera::Camera(World* world)
            : Entity(world),
            _frustum(this, &Camera::RecalculateFrustum)
        {
            _fov = (float)(M_PI / 4.0f);
            _aspectRatio = 1.3333f;
            _nearPlane = 0.1f;
            _farPlane = 4000.0;

            SetVisible(false);
        }

        static void SetPerspectiveMatrix(float fovy, float aspect, float zNear, float zFar)
        {
            float m[4][4];
            float sine, cose, cotangent, deltaZ;
            float radians = fovy / 2.0f;

            sincos(radians, &sine, &cose);

            deltaZ = zFar - zNear;
            if ((deltaZ == 0) || (sine == 0) || (aspect == 0))
                return;
            cotangent = cose / sine;

            ZeroMemory(m, sizeof(m));
            m[0][0] = cotangent / aspect;
            m[1][1] = cotangent;
            m[2][2] = -(zFar + zNear) / deltaZ;
            m[2][3] = -1;
            m[3][2] = -2 * zNear * zFar / deltaZ;
            m[3][3] = 0;
            glLoadMatrixf(&m[0][0]);
        }

        void Camera::LoadCameraTransform() const
        {
            glMatrixMode(GL_PROJECTION);
            SetPerspectiveMatrix(_fov, _aspectRatio, _nearPlane, _farPlane);

            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(gSwapAxisMatrix);

            const Entity* current = this;
            while (current != NULL)
            {
                current->ApplyInvTransform();
                current = current->GetParentEntity();
            }
        }

        void Camera::DoRender(const RendererContext& params)
        {
            if (GetWorld()->GetActiveCamera() != this)
            {
                Graphics::DrawFrame();
                glColor3f(1, 1, 1);
                Graphics::DrawFrustum(GetFrustum());
            }
        }

        /*
        Rotate camera so it looks at specific direction.
        */
        void Camera::LookAt(const Vector& direction, const Vector& up)
        {
            Vector x = - direction;
            Vector y;
            Vector z = up;

            y = z ^ x;
            z = x ^ y;

            Scalar len = y.Length();

            x.Normalize();
            if (len) y *= 1.0f / len;
            len = z.Length();
            if (len) z *= 1.0f / len;

            Matrix m;
            m.SetRows(x, y, z);
        
            QTransform tr = GetTransform();
            tr.Rotation.SetMatrix(m);
            SetTransform(tr);
        }
    }
}