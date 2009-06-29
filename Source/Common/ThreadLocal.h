#pragma once

#if defined(P3D_WINDOWS)
#include "Windows/Thread.h"
#endif

namespace P3D
{
    template<class T>
    class ThreadLocal
    {
    public:
        ThreadLocal()
        {
            _index = Impl::AllocTLSIndex();
        }

        bool IsValid() const { return _index != -1; }

        T* Get()
        {
            ASSERT(IsValid());
            return (T*)Impl::GetTLSValue(_index);
        }

        void Set(T* value)
        {
            ASSERT(IsValid());
            Impl::SetTLSValue(_index, value);
        }

    private:
        typedef Implementation::Thread Impl;
        Impl::TLSIndex _index;
    };
}