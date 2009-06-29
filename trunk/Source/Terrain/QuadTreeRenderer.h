#pragma once

#include "QuadTree.h"

namespace P3D
{
    namespace World
    {
        extern int gQuadTreeChecks;

        /*
        Assembles list of visible patches.
        */
        class QuadTreeRenderer
        {
        public:
            QuadTreeRenderer();

            /*
            Recursivly visit quad tree nodes and assemble list of visible nodes.
            */
            void Render(QuadTreeNodeBase* root, const Camera& camera, 
                const Transform& cameraToObj, const Transform& objToCam);

        public:
            /*
            Array of all visible leafs sorted in quad tree traversal manner.
            */
            std::vector<QuadTreeLeaf*> VisibleLeafs;

        private:
            enum VisibilityTestResult
            {
                FULLY_VISIBLE,
                PARTLY_VISIBLE,
                INVISIBLE
            };

            const Camera* _camera;
            Frustum _viewFrustum;
            Sphere _viewBoundingSphere;
            Transform _camToObj;
            Transform _objToCam;

            ushort _tick;
            byte _traversalOrder[4];

            /*
            Checks if the node is inside frustum.
            */
            VisibilityTestResult IsVisible(const QuadTreeNodeBase* node, int cullingMask, int& newCullingMask) const;

            /*
            Fills 'VisibleLeafs' with visible leafs of this node.
            if 'checkVisibility' is false, no frustum check perfomed.
            */
            forceinline void FillVisible(const QuadTreeNodeBase* node, bool checkVisibility, int cullingMask)
            {
                if (node->IsLeaf)
                    FillVisibleLeaf((QuadTreeLeaf*)node);
                else
                    FillVisibleNode((const QuadTreeNode*)node, checkVisibility, cullingMask);
            }

            /*
            Fills 'VisibleLeafs' with visible leafs of this node.
            if 'checkVisibility' is false, no frustum check perfomed.
            */
            void FillVisibleNode(const QuadTreeNode* node, bool checkVisibility, int cullingMask);

            /*
            Add leaf to the visible list.
            */
            forceinline void FillVisibleLeaf(QuadTreeLeaf* leaf)
            {
                leaf->LastVisibleTick = _tick;
                VisibleLeafs.push_back(leaf);
            }
        };
    }
}