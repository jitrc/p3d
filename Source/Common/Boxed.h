#pragma once

#include "Object.h"
#include "ObjectPool.h"

namespace P3D
{
    template<class T>
    void ObReinitialize(T& t) {}

    template<class T>
    void ObDeinitialize(T& t) {}

    template<>
    void ObReinitialize<ManualEvent>(ManualEvent& t);

    template<>
    void ObDeinitialize<ManualEvent>(ManualEvent& t);

    /*
    Wrap any class or POD into reusable reference countable object.
    */
    template<class T>
    class Boxed : 
        public ReusableObject
    {
    public:
        Boxed() {}
        Boxed(const T& t) : Data(t) {}

        T& operator=(const T& t)
        {
            Data = t;
            return Data;
        }

        T& Unbox() { return Data; }
        const T& Unbox() const { return Data; }

    protected:
        virtual void ReinitializeReusable() { ObReinitialize<T>(Data); }
        virtual void DeinitializeReusable() { ObDeinitialize<T>(Data); }

    private:
        T Data;
    };

    template<class T>
    Boxed<T>* Box(const T& t)
    {
        return new Boxed<T>(t);
    }

    template<class T>
    Boxed<T>* Box(const Boxed<T>& t)
    {
        return new Boxed<T>(t.Unbox());
    }
}