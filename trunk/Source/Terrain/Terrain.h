#pragma once

#include "TerrainVertex.h"
#include "TerrainPatch.h"
#include "QuadTreeRenderer.h"

namespace P3D
{
    namespace World
    {
        extern int gPatchRebuilds;

        class Terrain : 
            public Entity
        {
            friend class TerrainPatch;
            friend class PatchTessalator;

            static Logger logger;

        public:
            typedef ushort HeightMapPixel;

            Terrain(World* world);
            virtual ~Terrain();

            /*
            Load terraing from RAW 8-bit per channel file.
            */
            void Load(const wchar* file, int width, int height, float meshStepX, float meshStepY);

            inline Scalar GetVertexZ(uint x, uint y) const
            {
                return (_map[y* _mapSizeX + x] * 5000.0f / 65535.0f);
                // return sin(x * 0.05f) * cos(y * 0.05f) * 10.0f;
                // return 0;
            }

            inline Scalar GetVertexX(uint x) const
            {
                return x * _meshStepX - _centerX;
            }

            inline Scalar GetVertexY(uint y) const
            {
                return y * _meshStepY - _centerY;
            }

            inline int GetPointX(Scalar x) const
            {
                return (int)((x + _centerX) / _meshStepX);
            }

            inline int GetPointY(Scalar y) const
            {
                return (int)((y + _centerY) / _meshStepY);
            }

            /*
            Return position of the vertex.
            */
            inline void GetVertexPosition(uint x, uint y, Vector& vertex) const
            {
                vertex.x = GetVertexX(x);
                vertex.y = GetVertexY(y);
                vertex.z = GetVertexZ(x, y);
            }

        protected:
            override void DoRender(const RendererContext& params);
            override void RecalculateBoundingBox(AABB& box);

        private:
            static const int PATCH_SIZE = TerrainPatch::PATCH_SIZE;
            static const int MAX_VERTICES_IN_VB = ((4 * (PATCH_SIZE - 1) + 1) * (4 * (PATCH_SIZE - 1) + 1)) + 1;

            HeightMapPixel* _map;
            float _centerX, _centerY;
            float _meshStepX, _meshStepY;
            uint _mapSizeX, _mapSizeY;
            uint _sizeX, _sizeY;

            // memory stats
            int _memoryUsed;
            int _quadNodeCount;
            int _vbTotalSize;

            // Vertex buffer and area of its activiness :)
            struct PatchCluster
            {
                int Left, Down;
                int SizeX, SizeY;
                VertexBuffer* VB;
                IndexBuffer* IB;
                int IBSize;
            };

            // Area of active VB. 
            // See GetVertexIndex and SetIndexSelector.
            int _selLeft, _selSizeX, _selDown, _selSizeY;
            VertexBuffer* _curSel;

            // map of all clusters
            // managed by AllocateNewCluster and ReleaseClusters functions.
            typedef std::hash_map<VertexBuffer*, PatchCluster>::iterator ClustersIterator;
            std::hash_map<VertexBuffer*, PatchCluster> _clusters;

            // buffers currently active
            VertexBuffer* _activeBuffer;
            IndexBuffer* _activeIndexBuffer;

            uint _patchesSX, _patchesSY;
            TerrainPatch** _patches; // array of all patches
            QuadTreeNodeBase* _quadRoot; // quad tree of all patches

            // Object that can obtain list of visible patches
            QuadTreeRenderer _renderer;

            // Create all patches and build quad trees out of them.
            void CreatePatches();

            // Destroy all patches and quad tree.
            void DestroyPatches();

            // Build quad tree node.
            QuadTreeNodeBase* BuildQuadTree(int left, int right, int down, int up, PatchCluster* vb);

            // return true in case the quad node with sizes 'sx' and 'sz' should have VB attached.
            bool ShouldMakeNewVB(uint sx, uint sy) const;

            // makes vertex buffer active in case it is not active now
            inline void ActivateVertexBuffer(VertexBuffer* vb)
            {
                if (_activeBuffer != vb)
                {
                    if (_activeBuffer) _activeBuffer->Deactivate();
                    _activeBuffer = vb;
                    _activeBuffer->Activate();
                }
            }

            // makes vertex buffer active in case it is not active now
            inline void ActivateIndexBuffer(IndexBuffer* ib)
            {
                if (_activeIndexBuffer != ib)
                {
                    if (_activeIndexBuffer) _activeIndexBuffer->Deactivate();
                    _activeIndexBuffer = ib;
                    _activeIndexBuffer->Activate();
                }
            }

            // Allocate new vertex buffer for node with borders specified.
            PatchCluster* AllocateNewCluster(int left, int right, int down, int up);

            // Releases all allocated clusters
            void ReleaseClusters();

            // After the call to this function GetVertexIndex will return indices within vb.
            inline void SetIndexSelector(VertexBuffer* vb)
            {
                if (_curSel == vb) return;
                std::hash_map<VertexBuffer*, PatchCluster>::iterator it = _clusters.find(vb);
                ASSERT(it != _clusters.end());

                _curSel = vb;
                _selLeft = it->second.Left;
                _selSizeX = it->second.SizeX;
                _selDown = it->second.Down;
                _selSizeY = it->second.SizeY;
            }

            // return the index of the vertex in the active for selection VB
            inline ushort GetVertexIndex(uint x, uint y) const 
            {
                return (y - _selDown) * _selSizeX + (x - _selLeft);
            }

            // return full vertex description.
            void GetVertex(uint x, uint y, TerrainVertex& vertex) const;
        };
    }
}