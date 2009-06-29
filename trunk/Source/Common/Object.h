#pragma once

#include "Atomic.h"

namespace P3D
{
    /*
    Base class for objects that support reference counting mechanism and live on heap.
    */
    class Object
    {
    protected:
        virtual ~Object() 
        {
#ifdef _DEBUG
            _inDestructor = true;
#endif
        }
    public:
        /*
        Construct object with reference count set to 1.
        */
        Object()
        {
#ifdef _DEBUG
            _inDestructor = false;
#endif
            _refCount = 1;
        }
        
        /*
        Add one more reference to the object.
        Return new reference count.
        */
        virtual int AddRef()
        {
#ifdef _DEBUG
            ASSERT(!_inDestructor);
#endif
            return AtomicIncrement(&_refCount);
        }

        /*
        Remove reference.
        If reference count reaches zero object disposes.
        Return new reference counter.
        */
        virtual int Release()
        {
            if (AtomicDecrement(&_refCount) == 0)
            {
#ifdef _DEBUG
                _inDestructor = true;
#endif
                delete this;
                return 0;
            } else
                return _refCount;
        }

    protected:
        ALIGNED_VARIABLE(32, volatile long, _refCount);
#ifdef _DEBUG
        bool _inDestructor;
#endif
    };
}
