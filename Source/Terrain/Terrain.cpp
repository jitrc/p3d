#include "Includes.h"
#include "Terrain.h"

namespace P3D
{
    namespace World
    {
        Logger Terrain::logger(L"World.Terrain");

        int gPatchRebuilds = 0;

        Terrain::Terrain(World* world)
            : Entity(world)
        {
            _sizeX = 0;
            _sizeY = 0;
            _meshStepX = 0;
            _meshStepY = 0;
            _map = NULL;
            _patches = NULL;
            _patchesSX = _patchesSY = 0;
            _quadRoot = NULL;
            _selLeft = _selSizeX = _selDown = _selSizeY = 0;
            _curSel = NULL;
            _activeBuffer = NULL;
            _activeIndexBuffer = NULL;

            _memoryUsed = 0;
            _quadNodeCount = 0;
            _vbTotalSize = 0;

            //_program = NULL;
            //ShaderLoader loader;
            //_program = loader.Load("Shaders/Shader.xml");
        }

        Terrain::~Terrain()
        {
            DestroyPatches();
            if (_map) free(_map);
            //if (_program) _program->Release();
        }

        void Terrain::GetVertex(uint x, uint y, TerrainVertex& vertex) const
        {
            Vector pos;
            GetVertexPosition(x, y, pos);
            vertex.x = pos.x;
            vertex.y = pos.y;
            vertex.z = pos.z;

            vertex.lz = vertex.z;
            vertex.level = TerrainPatch::LOD_LEVELS;

            bool xBorder = (x % (TerrainPatch::PATCH_SIZE - 1)) == 0;
            bool yBorder = (y % (TerrainPatch::PATCH_SIZE - 1)) == 0;

            vertex.tx = float(x) / _sizeX;
            vertex.ty = 1.0f - float(y) / _sizeY;

            int step = (TerrainPatch::PATCH_SIZE - 1) / 2;
            int level = TerrainPatch::LOD_LEVELS - 1;

            while (true)
            {
                if ((x % step) == 0 && (y % step) == 0)
                {
                    // found lod where vertex is knot vertex
                    // classify
                    bool sx = (x % (step*2)) == 0;
                    bool sy = (y % (step*2)) == 0;

                    if (!sx && !sy) 
                        vertex.lz = (GetVertexZ(x + step, y - step) + GetVertexZ(x - step, y + step))/2;
                    if (sx && !sy)
                        vertex.lz = (GetVertexZ(x, y - step) + GetVertexZ(x, y + step))/2;
                    if (!sx && sy)
                        vertex.lz = (GetVertexZ(x - step, y) + GetVertexZ(x + step, y))/2;

                    if (!sx || !sy)
                    {
                        vertex.level = (float)level;
                        break;
                    }
                }
                if (step == 1)  break;
                step = step / 2;
                level--;
            }
        }

        void Terrain::Load(const wchar* file, int width, int height, float meshStepX, float meshStepY)
        {
            logger.info() << L"Loading " << width << L"x" << height 
                << L" terrain from '" << file << L"'...";

            // reset stats
            _memoryUsed = 0;
            _quadNodeCount = 0;
            _vbTotalSize = 0;

            _mapSizeX = width;
            _mapSizeY = height;

            _sizeX = ((width - 1) / TerrainPatch::PATCH_SIZE) * TerrainPatch::PATCH_SIZE + 1;
            _sizeY = ((height - 1) / TerrainPatch::PATCH_SIZE) * TerrainPatch::PATCH_SIZE + 1;

            _meshStepX = meshStepX;
            _meshStepY = meshStepY;

            _centerX = _sizeX * _meshStepX / 2.0f - _meshStepX / 2.0f;
            _centerY = _sizeY * _meshStepY / 2.0f - _meshStepY / 2.0f;

            // create and load map
            {
                logger.info() << L"Loading height map (" 
                    << width * height * sizeof(HeightMapPixel) << L" bytes)...";

                _map = (HeightMapPixel*)malloc(width * height * sizeof(HeightMapPixel));
                _memoryUsed += width * height * sizeof(HeightMapPixel);

                FILE* f = _wfopen(file, L"rb");
                fread(_map, width*height*sizeof(HeightMapPixel), 1, f);
                fclose(f);
            }

            CreatePatches();
        }

        void Terrain::ReleaseClusters()
        {
            for (ClustersIterator i = _clusters.begin(); i != _clusters.end(); i++)
            {
                i->second.VB->Release();
                i->second.IB->Release();
            }
            _clusters.clear();
        }

        bool Terrain::ShouldMakeNewVB(uint sx, uint sy) const
        { 
            uint vertexCount = ((sx * (PATCH_SIZE - 1) + 1) * (sy * (PATCH_SIZE - 1) + 1));
            return vertexCount < (uint)MAX_VERTICES_IN_VB;
        }

        // Allocate new vertex buffer for node with borders specified.
        Terrain::PatchCluster* Terrain::AllocateNewCluster(int left, int right, int down, int up)
        {
            // create VB
            VertexBuffer* vb = new VertexBuffer();
            TerrainVertex::BuildVertexDescription(vb->Element);
            vb->Initialize((right - left + 1) * (up - down + 1), BUFFER_STATIC);

            // create IB
            IndexBuffer* ib = new IndexBuffer();
            ib->Element.Size = sizeof(TerrainPatch::IndexType);

            // create cluster
            PatchCluster cluster;
            cluster.Left = left;
            cluster.Down = down;
            cluster.SizeX = right - left + 1;
            cluster.SizeY = up - down + 1;
            cluster.VB = vb;
            cluster.IB = ib;
            cluster.IBSize = 0;

            // add cluster
            _clusters[vb] = cluster;

            SetIndexSelector(vb); // make VB active for index selection
            BufferUpdater<VertexBuffer, TerrainVertex> v(vb);
            for (int x = left; x <= right; x++)
            {
                for (int y = down; y <= up; y++)
                {
                    GetVertex(x, y, v[GetVertexIndex(x, y)]);
                }
            }

            _vbTotalSize += (cluster.SizeX * cluster.SizeY) * sizeof(TerrainVertex);

            return &_clusters[vb];
        }

        void Terrain::CreatePatches()
        {
            _patchesSX = _sizeX / (PATCH_SIZE - 1);
            _patchesSY = _sizeY / (PATCH_SIZE - 1);

            logger.info() << L"Building " << _patchesSX << L"x" << _patchesSY
                << L" patch list (" 
                << _patchesSX * _patchesSY * (sizeof(void*) + sizeof(TerrainPatch))
                << L" bytes)...";

            if (_patchesSX == 0 || _patchesSY == 0) return;

            // create array of pointers
            _patches = new TerrainPatch*[_patchesSX * _patchesSY];
            _memoryUsed += (sizeof(void*) + sizeof(TerrainPatch)) * _patchesSX * _patchesSY;

            // create patches
            for (uint y = 0; y < _patchesSY; y++)
            {
                for (uint x = 0; x < _patchesSX; x++)
                {
                    _patches[y * _patchesSX + x] = new TerrainPatch(this, x * (PATCH_SIZE - 1), y * (PATCH_SIZE - 1));
                }
            }

            // link patches
            for (uint y = 0; y < _patchesSY; y++)
            {
                for (uint x = 0; x < _patchesSX; x++)
                {
                    TerrainPatch* cur = _patches[y * _patchesSX + x];
                    if (x < _patchesSX - 1) cur->_right = _patches[y * _patchesSX + x + 1];
                    if (x > 0) cur->_left = _patches[y * _patchesSX + x - 1];
                    if (y < _patchesSY - 1) cur->_down = _patches[(y + 1) * _patchesSX + x];
                    if (y > 0) cur->_up = _patches[(y - 1) * _patchesSX + x];
                }
            }

            // link into quad tree
            logger.info() << L"Building terrain quad tree...";
            _quadRoot = BuildQuadTree(0, _patchesSX, 0, _patchesSY, NULL);

            logger.info() << L"Creating index buffers...";
            for (ClustersIterator i = _clusters.begin(); i != _clusters.end(); i++)
                i->second.IB->Initialize(i->second.IBSize, BUFFER_DYNAMIC);

            // calculate total AABB
            logger.info() << L"Calculating bounding boxes...";
            _quadRoot->CalculateBoundingBox();

            logger.info() << L"Terrain building complete!";
            logger.info() << L"Total RAM used : " << _memoryUsed / 1024 << L" Kb.";
            logger.info() << L"Quad Nodes used: " << _quadNodeCount <<L" (" << _quadNodeCount*sizeof(QuadTreeNode) / 1024 << L" Kb total).";
            logger.info() << L"Vertex Buffers used: " << _clusters.size() << L" (" << _vbTotalSize / 1024 << L" Kb total).";
            logger.info() << L"Index Buffers used : " << _clusters.size() << L" (" 
                << _patchesSX*_patchesSY*TerrainPatch::MAX_INDICES_COUNT * sizeof(TerrainPatch::IndexType) / 1024 << L" Kb total).";
        }

        void Terrain::DestroyPatches()
        {
            if (!_patches) return;

            // release quad tree
            if (_quadRoot) _quadRoot->DeleteQuadNode();
            _quadRoot = NULL; 

            // delete patches
            for (uint i = 0; i < _patchesSY * _patchesSX; i++)
            {
                delete _patches[i];
            }
            delete[] _patches;
            _patches = NULL;

            // release vertex buffers
            ReleaseClusters();
        }

        QuadTreeNodeBase* Terrain::BuildQuadTree(int left, int right, int down, int up, PatchCluster* cluster)
        {
            PatchCluster* newCluster = NULL;
            if (cluster != NULL) 
                newCluster = cluster;
            else
            {
                int nodeSX = right - left;
                int nodeSY = up - down;
                if (ShouldMakeNewVB(nodeSX, nodeSY))
                    newCluster = AllocateNewCluster(left * (PATCH_SIZE - 1), right * (PATCH_SIZE - 1), down * (PATCH_SIZE - 1), up * (PATCH_SIZE - 1));
            }

            // leaf?
            if (((left + 1) == right) && ((down + 1)== up))
            {
                ASSERT(newCluster != NULL);

                TerrainPatch* cur = _patches[down * _patchesSX + left];

                cur->VB = newCluster->VB;
                cur->_indexOffset = newCluster->IBSize;
                cur->IB = newCluster->IB;
                newCluster->IBSize += TerrainPatch::MAX_INDICES_COUNT;

                return cur;
            }

            // new node
            _memoryUsed += sizeof(QuadTreeNode);
            _quadNodeCount++;

            QuadTreeNode* node = new QuadTreeNode();

            // center...
            int cx, cy;
            cx = (left + right) / 2;
            cy = (down + up) / 2;
            node->LeftDown = BuildQuadTree(left, max(cx, left+1), down, max(cy, down+1), newCluster);
            node->LeftUp = BuildQuadTree(left, max(cx, left+1), cy, up, newCluster);
            node->RightDown = BuildQuadTree(cx, right, down, max(cy, down+1), newCluster);
            node->RightUp = BuildQuadTree(cx, right, cy, up, newCluster);

            return node;
        }

        void Terrain::RecalculateBoundingBox(AABB& box)
        {
            if (_quadRoot)
                box = _quadRoot->BoundingBox;
            else
                box.SetImpossible();
        }

        void Terrain::DoRender(const RendererContext& params)
        {
            if (!_patches) return;

            gPatchRebuilds = 0;

            // get camera
            const Camera* camera = GetWorld()->GetActiveCamera();

            // get transform from camera space to terrain space
            QTransform camToWorld = camera->GetTransformToWorldSpace();
            QTransform worldToObj = GetTransformToWorldSpace();
            worldToObj.Invert();
            Transform camToObj(worldToObj * camToWorld);

            Transform objToCam = camToObj;
            objToCam.Invert();

            _renderer.Render(_quadRoot, *camera, camToObj, objToCam);

            _activeBuffer = NULL;
            _activeIndexBuffer = NULL;

            // calculate errors
            for (uint i = 0; i < _renderer.VisibleLeafs.size(); ++i)
            {
                TerrainPatch* cur = (TerrainPatch*)_renderer.VisibleLeafs[i];
                cur->CalculateLOD(camToObj);
            }

            // make patches with common side have lods differ no more than by 1
            // TODO: optimize!
            while (true)
            {
                int changes = 0;
                for (uint i = 0; i < _renderer.VisibleLeafs.size(); ++i)
                {
                    TerrainPatch* cur = (TerrainPatch*)_renderer.VisibleLeafs[i];
                    changes += cur->NormalizeLOD();
                }
                if (changes <= 0) break;
            }

            // rebuild IB for new lod levels
            for (uint i = 0; i < _renderer.VisibleLeafs.size(); ++i)
            {
                TerrainPatch* cur = (TerrainPatch*)_renderer.VisibleLeafs[i];
                if (cur->ShouldRebuidIB()) cur->RebuildIndexBuffer();
            }

            //if (_program) _program->Activate();

            // render all patches
            for (uint i = 0; i < _renderer.VisibleLeafs.size(); ++i)
            {
                TerrainPatch* cur = (TerrainPatch*)_renderer.VisibleLeafs[i];
                cur->Render();
            }

            //if (_program) _program->Deactivate();

            if (_activeBuffer)
            {
                _activeBuffer->Deactivate();
                _activeBuffer = NULL;
            }

            if (_activeIndexBuffer)
            {
                _activeIndexBuffer->Deactivate();
                _activeIndexBuffer = NULL;
            }
        }
    }
}