#pragma once

namespace P3D
{
    namespace World
    {
        struct TerrainVertex
        {
            union
            {
                float coord[3];
                struct
                {
                    float x, y, z;
                };
            };

            union
            {
                float lodParams[2];
                struct
                {
                    float lz, level;
                };
            };

            union
            {
                float texCoords[2];
                struct
                {
                    float tx, ty;
                };
            };

            float selectIndex;

            static void BuildVertexDescription(VertexDescription& desc)
            {
                desc.Clear();
                desc.Size = sizeof(TerrainVertex);
                desc.Add(VD_COORD, &TerrainVertex::coord);
                desc.Add(VD_TEXTURE0, &TerrainVertex::texCoords);
                desc.AddAttrib(1, &TerrainVertex::lodParams);
                desc.AddAttrib(2, &TerrainVertex::selectIndex);
            }
        };
    }
}