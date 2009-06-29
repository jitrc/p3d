#include "Includes.h"
#include "Boxed.h"

namespace P3D
{
    template<>
    void ObReinitialize<ManualEvent>(ManualEvent& t)
    { }

    template<>
    void ObDeinitialize<ManualEvent>(ManualEvent& t)
    { 
        t.Clear();
    }
}