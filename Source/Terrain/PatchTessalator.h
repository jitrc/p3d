#pragma once

namespace P3D
{
    namespace World
    {
        class PatchTessalator
        {
            const int _patchX, _patchY;
            TerrainPatch::IndexType* _ib;
            TerrainPatch::IndexType* _ptr;
            const Terrain* _parent;
            const int step;
            const bool _left, _right, _up, _down;
            const int maxCount;

            int _lastIndex;
        public:
            PatchTessalator(TerrainPatch::IndexType* ib, Terrain* parent, int patchX, int patchY, int step, 
                bool left, bool right, bool up, bool down)
                : _ib(ib),  _parent(parent), _patchX(patchX), _patchY(patchY), step(step),
                _left(left), _right(right), _up(up), _down(down), 
                maxCount(TerrainPatch::PATCH_SIZE - step)
            { 
                _lastIndex = -1;
                _ptr = _ib;
            }

            forceinline int GetWrittenIndicesCount() const
            {
                return _ptr - _ib;
            }

            forceinline void Vertex(int x, int y)
            {
                _lastIndex = _parent->GetVertexIndex(_patchX + x, _patchY + y);
                *_ptr = _lastIndex;
                _ptr++;
            }

            forceinline void LastVertexAgain()
            {
                if (_lastIndex != -1)
                {
                    *_ptr = _lastIndex;
                    _ptr++;
                }
            }

            void RenderStrip(int y)
            {
                Vertex(0, y);
                int x = 0;
                const int count = _right ? (maxCount - step): (maxCount + step);
                if (_left)
                {
                    LastVertexAgain();
                    LastVertexAgain();
                    Vertex(x + step, y);
                    Vertex(x, y + step/2);
                    Vertex(x + step, y + step);
                    Vertex(x , y + step);
                    Vertex(x + step, y + step);
                    LastVertexAgain();
                    x+=step;
                }
                for (x; x < count; x+=step)
                {
                    Vertex(x, y);
                    Vertex(x, y + step);
                }
                if (_right)
                {
                    Vertex(x, y);
                    Vertex(x, y + step);
                    Vertex(x, y);
                    Vertex(x + step, y + step / 2);
                    Vertex(x + step, y);
                    Vertex(x + step, y  + step / 2);
                    LastVertexAgain();
                    Vertex(x, y + step);
                    Vertex(x + step, y + step);
                    LastVertexAgain();
                }
            }

            void RenderTopStrip()
            {
                Vertex(0, 0);
                int x = 0;
                const int count = _right ? (maxCount - step) : maxCount;
                if (_left)
                {
                    Vertex(x,          0);
                    Vertex(x,          step / 2);
                    Vertex(x + step/2, 0);
                    Vertex(x,          step);
                    Vertex(x + step/2, 0);
                    Vertex(x + step, step);
                    x+=step;
                }
                for (x; x < count; x+=step)
                {
                    Vertex(x,          0);
                    Vertex(x,          step);
                    Vertex(x + step/2, 0);
                    Vertex(x + step, step);
                }
                if (_right)
                {
                    Vertex(x,          0);
                    Vertex(x,          step);
                    Vertex(x + step/2, 0);
                    Vertex(x + step,   step);
                    Vertex(x + step/2, 0);
                    Vertex(x + step,   step/2);
                    Vertex(x + step,   0);
                    LastVertexAgain();
                } else
                {
                    Vertex(x,          0);
                    Vertex(x,          step);
                }
            }

            void RenderBottomStrip()
            {
                const int y = TerrainPatch::PATCH_SIZE - step - 1;
                Vertex(0, y);
                int x = 0;
                const int count = _right ? (maxCount - step) : maxCount;
                if (_left)
                {
                    Vertex(x,          y);
                    Vertex(x,          y + step / 2);
                    Vertex(x + step / 2,          y + step);
                    Vertex(x,          y + step);
                    LastVertexAgain();
                    LastVertexAgain();

                    Vertex(x,          y);
                    Vertex(x + step/2, y + step);
                    x+=step;
                }
                for (x; x < count; x+=step)
                {
                    Vertex(x,          y);
                    Vertex(x,          y + step);
                    Vertex(x,          y);
                    Vertex(x + step/2, y + step);
                }
                if (_right)
                {
                    Vertex(x,          y);
                    Vertex(x,          y + step);
                    Vertex(x,          y);
                    Vertex(x + step/2, y + step);

                    Vertex(x + step,   y);
                    Vertex(x + step,   y + step / 2);
                    LastVertexAgain(); 
                    Vertex(x + step/2, y + step);
                    Vertex(x + step,   y + step);
                } else
                {
                    Vertex(x,          y);
                    Vertex(x,          y + step);
                }
            }


            void RenderWorstLOD()
            {
                const int	hstep = step / 2;
                int			code = (_left ? 1 : 0) | (_right ? 2 : 0) | (_up ? 4 : 0) | (_down ? 8 : 0);

                // use the created code for fast selection of the case to build...
                //
                switch(code)
                {
                case 0:
                    Vertex(0,0);
                    LastVertexAgain();
                    Vertex(0,step);
                    Vertex(step,0);
                    Vertex(step,step);
                    break;

                case 1:
                    Vertex(0,0);
                    LastVertexAgain();
                    Vertex(0,hstep);
                    Vertex(step,0);
                    Vertex(0,step);
                    Vertex(step,step);
                    LastVertexAgain();
                    break;

                case 2:
                    Vertex(step,step);
                    LastVertexAgain();
                    Vertex(step,hstep);
                    Vertex(0,step);
                    Vertex(step,0);
                    Vertex(0,0);
                    LastVertexAgain();
                    break;

                case 3:
                    Vertex(0,0);
                    LastVertexAgain();
                    Vertex(0,hstep);
                    Vertex(step,0);
                    Vertex(0,step);
                    Vertex(step,hstep);
                    Vertex(step,step);
                    break;

                case 4:
                    Vertex(0,0);
                    Vertex(hstep,0);
                    Vertex(0,step);
                    Vertex(step,0);
                    Vertex(step,step);
                    break;

                case 5:
                    Vertex(0,0);
                    LastVertexAgain();
                    Vertex(0,hstep);
                    Vertex(hstep,0);
                    Vertex(0,step);
                    Vertex(step,0);
                    Vertex(step,step);
                    break;

                case 6:
                    Vertex(0,0);
                    LastVertexAgain();
                    Vertex(0,step);
                    Vertex(hstep,0);
                    Vertex(step,0);
                    LastVertexAgain();
                    Vertex(0,step);
                    Vertex(step,hstep);
                    Vertex(step,step);
                    break;

                case 7:
                    Vertex(step,step);
                    LastVertexAgain();
                    Vertex(step,hstep);
                    Vertex(0,step);
                    Vertex(step,0);
                    Vertex(0,hstep);
                    Vertex(hstep,0);
                    Vertex(0,0);
                    LastVertexAgain();
                    break;

                case 8:
                    Vertex(step,step);
                    Vertex(hstep,step);
                    Vertex(step,0);
                    Vertex(0,step);
                    Vertex(0,0);
                    break;

                case 9:
                    Vertex(step,step);
                    LastVertexAgain();
                    Vertex(step,0);
                    Vertex(hstep,step);
                    Vertex(0,step);
                    LastVertexAgain();
                    Vertex(step,0);
                    Vertex(0,hstep);
                    Vertex(0,0);
                    break;

                case 10:
                    Vertex(step,step);
                    LastVertexAgain();
                    Vertex(step,hstep);
                    Vertex(hstep,step);
                    Vertex(step,0);
                    Vertex(0,step);
                    Vertex(0,0);
                    break;

                case 11:
                    Vertex(0,0);
                    LastVertexAgain();
                    Vertex(0,hstep);
                    Vertex(step,0);
                    Vertex(0,step);
                    Vertex(step,hstep);
                    Vertex(hstep,step);
                    Vertex(step,step);
                    LastVertexAgain();
                    break;

                case 12:
                    Vertex(0,0);
                    Vertex(hstep,0);
                    Vertex(0,step);
                    Vertex(step,0);
                    Vertex(hstep,step);
                    Vertex(step,step);
                    LastVertexAgain();
                    break;

                case 13:
                    Vertex(0,0);
                    LastVertexAgain();
                    Vertex(0,hstep);
                    Vertex(hstep,0);
                    Vertex(0,step);
                    Vertex(step,0);
                    Vertex(hstep,step);
                    Vertex(step,step);
                    LastVertexAgain();
                    break;

                case 14:
                    Vertex(step,step);
                    LastVertexAgain();
                    Vertex(step,hstep);
                    Vertex(hstep,step);
                    Vertex(step,0);
                    Vertex(0,step);
                    Vertex(hstep,0);
                    Vertex(0,0);
                    LastVertexAgain();
                    break;

                case 15:
                    Vertex(0,0);
                    LastVertexAgain();
                    Vertex(0,hstep);
                    Vertex(hstep,0);
                    Vertex(0,step);
                    Vertex(step,0);
                    Vertex(hstep,step);
                    Vertex(step,hstep);
                    Vertex(step,step);
                    break;
                }
            }

            forceinline void RenderStrips()
            {
                int y = 0;
                const int count = _down ? (maxCount - step) : maxCount;
                if (_up)
                {
                    RenderTopStrip();
                    if (y+step != maxCount) LastVertexAgain();
                    y += step;
                }

                for (; y < count; y+=step)
                {
                    RenderStrip(y);
                    if (y+step != maxCount) LastVertexAgain();
                }

                if (_down)
                    RenderBottomStrip();
            }

            forceinline void RenderPatch()
            {
                if (step == TerrainPatch::PATCH_SIZE - 1)
                    RenderWorstLOD();
                else
                    RenderStrips();
            }
        };
    }
}

