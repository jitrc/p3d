#include "Includes.h"
#include "ObjectPool.h"
#include "Boxed.h"
#include "Synchronization.h"
#include "Thread.h"

namespace P3D
{
    DEFINE_OBJECT_POOL(Boxed<ManualEvent>);
}