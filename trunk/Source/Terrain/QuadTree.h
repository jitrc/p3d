#pragma once

namespace P3D
{
    namespace World
    {
        class QuadTreeNodeBase;
        class QuadTreeNode;
        class QuadTreeLeaf;

        /*
        Base class for leaf and node of the quad tree.
        Has bounding box and bounding sphere.
        */
        class QuadTreeNodeBase
        {
        public:
            /*
            Calculate, store and return AABB of the entire node.
            */
            virtual const AABB& CalculateBoundingBox() = 0;

            /*
            Calculate, store and return bounding sphere of the node.
            AABB is already known by this moment so it safe to get bounding sphere from aabb.
            */
            virtual const Sphere& CalculateBoundingSphere();

            /*
            Recursivly delete the tree.
            */
            virtual void DeleteQuadNode() = 0;
        public:
            AABB BoundingBox;
            Sphere BoundingSphere;
            bool IsLeaf;
        };

        /*
        Node that have childs.
        */
        class QuadTreeNode : public QuadTreeNodeBase
        {
        public:
            QuadTreeNode()
            {
                LeftUp = NULL;
                LeftDown = NULL;
                RightUp = NULL;
                RightDown = NULL;
                IsLeaf = false;
            }

            override const AABB& CalculateBoundingBox();

            /*
            Recursivly delete the tree.
            */
            override void DeleteQuadNode();

        public:
            union
            {
                struct 
                {
                    QuadTreeNodeBase* LeftUp;
                    QuadTreeNodeBase* LeftDown;
                    QuadTreeNodeBase* RightUp;
                    QuadTreeNodeBase* RightDown;
                };
                QuadTreeNodeBase* Childs[4];
            };
        };


        /*
        Leaf node of the quad tree.
        */
        class QuadTreeLeaf : public QuadTreeNodeBase
        {
        public:
            inline QuadTreeLeaf()
            { 
                VB = NULL; 
                IB = NULL;
                LastVisibleTick = 0;
                IsLeaf = true;
            }

        public:
            VertexBuffer* VB; // vertex buffer containing geometry of the node
            IndexBuffer* IB; // index buffer containing geometry of the node
            ushort LastVisibleTick; // tick of the renderer when we were visible last time
        };
    }
}