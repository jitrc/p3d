#include "Includes.h"
#include "QuadTree.h"

namespace P3D
{
    namespace World
    {
        const Sphere& QuadTreeNodeBase::CalculateBoundingSphere()
        {
            // get bounding sphere from bounding box
            Vector diag = BoundingBox.Max() - BoundingBox.Min();
            BoundingSphere.SetCenter((BoundingBox.Max() + BoundingBox.Min()) * 0.5f);
            BoundingSphere.SetRadius(diag.Length() * 0.5f);
            return BoundingSphere;
        }

        const AABB& QuadTreeNode::CalculateBoundingBox()
        {
            BoundingBox.SetImpossible();
            for (int i = 0; i < 4; i++)
                if (Childs[i]) BoundingBox.Enlarge(Childs[i]->CalculateBoundingBox());
            CalculateBoundingSphere();
            return BoundingBox;
        }

        void QuadTreeNode::DeleteQuadNode()
        {
            for (int i = 0; i < 4; i++)
                if (Childs[i]) Childs[i]->DeleteQuadNode();
            delete this; // kill me
        }
    }
}