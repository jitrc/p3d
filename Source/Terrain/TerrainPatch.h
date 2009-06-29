#pragma once

#include "TerrainVertex.h"
#include "QuadTree.h"

namespace P3D
{
    namespace World
    {
        class Terrain;

        const Scalar TERRAIN_ERROR_THRESHOLD = 0.2f;

        /*
        NxN piece of terrain.
        Has index buffer.
        */
        class TerrainPatch : public QuadTreeLeaf
        {
            friend class Terrain;

        public:
            static const int LOD_LEVELS = 5;

            static const int PATCH_SIZE = (1 << LOD_LEVELS) + 1; // 2 ^ LOD_LEVELS + 1
            static const int MAX_INDICES_COUNT = (2 * (PATCH_SIZE - 1) * (PATCH_SIZE + 1) - 2) + PATCH_SIZE * 5;

            typedef ushort IndexType;

        public:
            TerrainPatch(Terrain* parent, int x, int y);
            ~TerrainPatch();

            /*
            Inherited from QuadTreeLeaf.
            */
            override const AABB& CalculateBoundingBox();

            /*
            Inherited from QuadTreeLeaf.
            Do nothing.
            */
            override void DeleteQuadNode() {};

            /*
            Render the patch.
            All visibility check already has been done.
            */
            void Render();

        protected:
            Terrain* _parent;
            ushort _x, _y; // location in the complete terrain

            float _LOD;
            byte _tessalationLevel;
            byte _curTessalationLevel; // lod of the current IB

            ushort _indecesCount; // how many indices to render
            IndexType _indexOffset;

            Vector _center; // center of the patch
            clock_t _nextUpdate;

            // byte mask with more detailes sides of current mesh
            enum
            {
                SIDE_LEFT	= (1 << 1),
                SIDE_RIGHT  = (1 << 2),
                SIDE_UP		= (1 << 3),
                SIDE_DOWN	= (1 << 4)
            };
            byte _meshSides;

            // neightbors
            TerrainPatch* _up;
            TerrainPatch* _down;
            TerrainPatch* _left;
            TerrainPatch* _right;

            // build index buffer for curent LOD value.
            void RebuildIndexBuffer(bool print = false);

            // Calculate and store LOD value (from 0.0 for best mesh to LOD_LEVELS for worst)
            void CalculateLOD(const Transform& fromCameraSpace);

            // make patches with common side have lods differ no more than by 1
            inline bool NormalizeLOD()
            {
                int l = LOD_LEVELS, r = LOD_LEVELS, u = LOD_LEVELS, d = LOD_LEVELS;
                if (_left && _left->LastVisibleTick == LastVisibleTick) l = _left->_tessalationLevel;
                if (_right && _right->LastVisibleTick == LastVisibleTick) r = _right->_tessalationLevel;
                if (_up && _up->LastVisibleTick == LastVisibleTick) u = _up->_tessalationLevel;
                if (_down && _down->LastVisibleTick == LastVisibleTick) d = _down->_tessalationLevel;

                int nt = Min(Min(l + 1, r + 1), Min(u + 1, d + 1));
                if (_tessalationLevel > nt)
                {
                    _tessalationLevel = nt;
                    return true;
                } else
                    return false;
            }

            /*
            Return true when current LOD changed or any neighborns LOD changed.
            */
            inline bool ShouldRebuidIB() const 
            { 
                if (_tessalationLevel != _curTessalationLevel) return true;

                byte newMeshSides = 0;

                if ((_up!=NULL) && (_up->LastVisibleTick == LastVisibleTick) && (_up->_tessalationLevel < _tessalationLevel))
                    newMeshSides |= SIDE_UP;

                if ((_down!=NULL) && (_down->LastVisibleTick == LastVisibleTick) && (_down->_tessalationLevel < _tessalationLevel))
                    newMeshSides |= SIDE_DOWN;

                if ((_left!=NULL) && (_left->LastVisibleTick == LastVisibleTick) && (_left->_tessalationLevel < _tessalationLevel))
                    newMeshSides |= SIDE_LEFT;

                if ((_right!=NULL) && (_right->LastVisibleTick == LastVisibleTick) && (_right->_tessalationLevel < _tessalationLevel))
                    newMeshSides |= SIDE_RIGHT;

                return (newMeshSides != _meshSides);
            }
        };
    }
}