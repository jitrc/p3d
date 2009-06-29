#pragma once

namespace P3D
{
    extern int g_PolygonCounter;
    extern int g_QuaternionToMatrixConversion;
    extern int g_MatrixToQuaternionConversion;
    extern int g_MatrixMultiply;
    extern int g_MatrixInverse;
    extern int g_QuaternionMultiply;
    extern int g_VectorMatrixTransform;
    extern int g_EntitiesRendered;

    inline void IncCounter(int& c) { c++; }

    inline void ClearCounters()
    {
        g_PolygonCounter = 0;
        g_QuaternionToMatrixConversion = 0;
        g_MatrixToQuaternionConversion = 0;
        g_MatrixMultiply = 0;
        g_MatrixInverse = 0;
        g_QuaternionMultiply = 0;
        g_VectorMatrixTransform = 0;
        g_EntitiesRendered = 0;
    }
}