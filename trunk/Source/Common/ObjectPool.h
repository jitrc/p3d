#pragma once

#include "Object.h"
#include "Synchronization.h"
#include "Singleton.h"

namespace P3D
{
    class ReusableObject;
    class ObjectPoolBase;
    template<class T> class ObjectPool;

    class ReusableObject 
        : public Object
    {
    public:
        override int AddRef();
        override int Release();

    protected:
        ReusableObject();

        virtual void ReinitializeReusable() {}
        virtual void DeinitializeReusable() {}

    private:
        friend class ObjectPoolBase;

        ObjectPoolBase* _owner;

        inline void SetOwner(ObjectPoolBase* pool) { _owner = pool; }
    };

    class ObjectPoolBase
    {
        friend class ReusableObject;
    public:
        ~ObjectPoolBase();

        ReusableObject* CreateObject();
        void Clear();

    protected:
        virtual ReusableObject* CreateObjectInstance() = 0;

    private:
        void Collect(ReusableObject* obj);

    private:
        Lock _lock;
        typedef std::vector<ReusableObject*> ObjectList;
        ObjectList _free;
    };

    /*
    Pool of free objects of type T.
    T should be inherited from ReusableObject
    */
    template<class T>
    class ObjectPool : 
        public ObjectPoolBase,
        public Singleton<ObjectPool<T> >
    {
        DECLARE_SINGLETON(ObjectPool<T>);

    public:
        T* CreateObject() { return (T*)ObjectPoolBase::CreateObject(); }
    private:
        override ReusableObject* CreateObjectInstance() { return new T(); }
    };

    template<class T>
    T* CreateObjectInPool()
    {
        return ObjectPool<T>::GetInstance().CreateObject();
    }

    #define DEFINE_OBJECT_POOL(Type)\
    ObjectPool< Type >* Singleton< ObjectPool< Type > >::_instance = NULL;\
    Lock Singleton< ObjectPool< Type > >::_lock;
}