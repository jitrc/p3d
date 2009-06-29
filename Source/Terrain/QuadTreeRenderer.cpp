#include "Includes.h"
#include "QuadTreeRenderer.h"

namespace P3D
{
    namespace World
    {
        int gQuadTreeChecks = 0;

        static forceinline void GetTraversalOrder(const Vector& cam, byte* traversalOrder)
        {
            if (cam.x > 0)
            {
                if (cam.y > 0)
                {
                    traversalOrder[0] = 2;
                    traversalOrder[1] = 0;
                    traversalOrder[2] = 3;
                    traversalOrder[3] = 1;
                } else
                {
                    traversalOrder[0] = 3;
                    traversalOrder[1] = 1;
                    traversalOrder[2] = 2;
                    traversalOrder[3] = 0;
                }
            } else
            {
                if (cam.y > 0)
                {
                    traversalOrder[0] = 0;
                    traversalOrder[1] = 1;
                    traversalOrder[2] = 2;
                    traversalOrder[3] = 3;
                } else
                {
                    traversalOrder[0] = 1;
                    traversalOrder[1] = 0;
                    traversalOrder[2] = 3;
                    traversalOrder[3] = 2;
                }
            }
        }

        QuadTreeRenderer::QuadTreeRenderer()
        {
            _tick = 0;
        }

        void QuadTreeRenderer::Render(QuadTreeNodeBase* root, const Camera& camera, const Transform& camToObj, const Transform& objToCam)
        {
            // obtain from camera
            _camera = &camera;
            _viewFrustum = camera.GetFrustum();
            _viewBoundingSphere = camera.GetBoundingSphere();

            // transform to quad tree object space
            _viewFrustum.Transform(camToObj);
            _viewBoundingSphere.Transform(camToObj);

            _camToObj = camToObj;
            _objToCam = objToCam;

            // obtain correct order for quad tree traversal
            GetTraversalOrder(camToObj.Translation, _traversalOrder);

            // clear current list
            VisibleLeafs.clear();

            _tick++; // update tick so all previous visible leafs are no longer visible
            gQuadTreeChecks = 0; // reset counter

            // start recursive check
            if (root)
            {
                int newMask = 0;
                VisibilityTestResult res = IsVisible(root, (1 << 6) - 1, newMask);
                if (res == INVISIBLE) return;
                if (res == FULLY_VISIBLE) FillVisible(root, false, newMask);
                if (res == PARTLY_VISIBLE) FillVisible(root, true, newMask);
            }
        }

        QuadTreeRenderer::VisibilityTestResult QuadTreeRenderer::IsVisible(const QuadTreeNodeBase* node, int cullingMask, int& newCullingMask) const
        {
            gQuadTreeChecks++;

            ///////////////////////
            /// Frustum test

            // Spheres test
            Vector distance = _viewBoundingSphere.GetCenter() - node->BoundingSphere.GetCenter();
            float distanceSquare = distance.LengthSquared();
            float radiusSum = _viewBoundingSphere.GetRadius() + node->BoundingSphere.GetRadius();
            if (distanceSquare > radiusSum * radiusSum) return INVISIBLE;

            // AABB test here....
            IntersectionResult res;
            res = _viewFrustum.Intersects(node->BoundingBox, cullingMask, newCullingMask);

            if (res == OUTSIDE) return INVISIBLE;
            if (res == INSIDE) return FULLY_VISIBLE;

            return PARTLY_VISIBLE;
        }

        void QuadTreeRenderer::FillVisibleNode(const QuadTreeNode* node, bool checkVisibility, int cullingMask)
        {
            if (!checkVisibility)
            {
                for (int i = 0; i < 4; i++)
                {
                    QuadTreeNodeBase* child = node->Childs[_traversalOrder[i]];
                    if (child) 
                    {
                        FillVisible(child, false, 0);
                    }
                }
            } else
            {
                for (int i = 0; i < 4; i++)
                {
                    QuadTreeNodeBase* child = node->Childs[_traversalOrder[i]];
                    if (child)
                    {
                        int newCullingMask;
                        VisibilityTestResult res = IsVisible(child, cullingMask, newCullingMask);
                        if (res == INVISIBLE) continue;
                        if (res == FULLY_VISIBLE) FillVisible(child, false, 0);
                        if (res == PARTLY_VISIBLE) FillVisible(child, true, newCullingMask);
                    }
                }
            }
        }
    }
}