#include "Includes.h"
#include "Terrain.h"
#include "TerrainPatch.h"

#include "PatchTessalator.h"

namespace P3D
{
    namespace World
    {
        TerrainPatch::TerrainPatch(Terrain* parent, int x, int y)
        {
            _parent = parent;
            _x = x;
            _y = y;
            _LOD = 0;
            _tessalationLevel = 0;
            _meshSides = 0;
            _curTessalationLevel = -1;
            _nextUpdate = -1;

            // this later set by Terrain
            _left = _down = _up = _right = NULL; 
        }

        TerrainPatch::~TerrainPatch()
        {
        }

        void TerrainPatch::CalculateLOD(const Transform& fromCameraSpace)
        {
            clock_t now = clock();
            if (now > _nextUpdate)
            {
                _nextUpdate = now + random(1000, 5000) * CLOCKS_PER_SEC / 1000;

                float len = (_center - fromCameraSpace.Translation).Length();
                _LOD = 1 / 25.0f * Power(len / 1000.0f, 3); // when to switch to worse level of detail

                if (_LOD > LOD_LEVELS) _LOD = LOD_LEVELS;
                _tessalationLevel = (int)_LOD;
            }
            //_LOD = 0;
        }

        void TerrainPatch::RebuildIndexBuffer(bool print)
        {
            _meshSides = 0;
            _curTessalationLevel = _tessalationLevel;

            // say that we want select indices from that VB
            _parent->SetIndexSelector(VB);
            _parent->ActivateIndexBuffer(IB); // and index buffer

            int step = 1 << _tessalationLevel;
            if (step > PATCH_SIZE - 1) step = PATCH_SIZE - 1;

            bool up = false;
            if ((_up!=NULL) && (_up->LastVisibleTick == LastVisibleTick) && (_up->_tessalationLevel < _tessalationLevel))
            {
                up = true;
                _meshSides |= SIDE_UP;
            }

            bool down = false;
            if ((_down!=NULL) && (_down->LastVisibleTick == LastVisibleTick) && (_down->_tessalationLevel< _tessalationLevel))
            {
                down = true;
                _meshSides |= SIDE_DOWN;
            }

            bool left = false;
            if ((_left!=NULL) && (_left->LastVisibleTick == LastVisibleTick) && (_left->_tessalationLevel < _tessalationLevel))
            {
                left = true;
                _meshSides |= SIDE_LEFT;
            }

            bool right = false;
            if ((_right!=NULL) && (_right->LastVisibleTick == LastVisibleTick) && (_right->_tessalationLevel < _tessalationLevel))
            {
                right = true;
                _meshSides |= SIDE_RIGHT;
            }

            static IndexType indexBuffer[MAX_INDICES_COUNT];
            PatchTessalator renderer(indexBuffer, _parent, _x, _y, step, left, right, up, down);
            renderer.RenderPatch();

            _indecesCount = renderer.GetWrittenIndicesCount();
            IB->Copy(indexBuffer, _indecesCount, _indexOffset);

            gPatchRebuilds++;
        }

        void TerrainPatch::Render()
        {
            _parent->ActivateVertexBuffer(VB); // activates correct vertex buffer
            _parent->ActivateIndexBuffer(IB); // and index buffer

            glColor3f(1, 1, 1);
            Primitive::Render(PRIMITIVE_TRIANGLE_STRIP, _indexOffset, _indecesCount, NULL, NULL, sizeof(IndexType));

            //glColor3f(1, 0, 0);
            //glBegin(GL_LINES);
            //glVertex3f(_center.x, _center.y, _center.z);
            //glVertex3f(_center.x + 100 * _planeNormal.x, _center.y + 100 * _planeNormal.y, _center.z + 100 * _planeNormal.z);
            //glEnd();

            glColor3f(0, 1, 0);
            Vector loc = _center;
            OutputText(loc.x, loc.y, loc.z + 10, "%.1f", _LOD);
        }

        const AABB& TerrainPatch::CalculateBoundingBox()
        {
            BoundingBox.SetZero();
            Vector minVertex, maxVertex;
            _parent->GetVertexPosition(_x, _y, minVertex);
            _parent->GetVertexPosition(_x + PATCH_SIZE - 1, _y + PATCH_SIZE - 1, maxVertex);
            Scalar maxZ = -1e10;
            Scalar minZ = +1e10;

            Vector center;
            center.Set(0, 0, 0);

            for (int x = 0; x < PATCH_SIZE; x++)
            {
                for (int y = 0; y < PATCH_SIZE; y++)
                {
                    Vector pos;
                    _parent->GetVertexPosition(_x + x, _y + y, pos);
                    if (maxZ < pos.z) maxZ = pos.z;
                    if (minZ > pos.z) minZ = pos.z;
                    center += pos;
                }
            }
            BoundingBox.Min().Set(minVertex.x, minVertex.y, 0);
            BoundingBox.Max().Set(maxVertex.x, maxVertex.y, maxZ);

            CalculateBoundingSphere();

            _center = center * (1.0f /(PATCH_SIZE * PATCH_SIZE));

            return BoundingBox;
        }
    }
}