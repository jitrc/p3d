#pragma once

#include "Synchronization.h"
#include "AtExitManager.h"

namespace P3D
{
    /*
    Helper class to define singletons.
    */
    template<class T>
    class Singleton
    {
    protected:
        typedef T SingletonType;

        static T& GetInstance()
        {
            AutoLock lock(_lock);
            if (!_instance) 
            {
                _instance = new T();
                AtExitManager::RegisterAtExit(&Singleton<T>::OnExit);
            }
            return *_instance;
        }
    private:
        static T* _instance;
        static Lock _lock;

        /*
        Callback to be called on program exit.
        */
        static void OnExit() 
        {
            if (_instance)
            {
                delete _instance;
                _instance = NULL;
            }
        }
    };

#define DECLARE_SINGLETON(Type) \
    public: \
        friend class Singleton< Type >; \
        static Type& GetInstance() { return Singleton< Type >::GetInstance(); }


#define DEFINE_SINGLETON(Type) \
    Type* Singleton< Type >::_instance = NULL;\
    Lock Singleton< Type >::_lock;
}