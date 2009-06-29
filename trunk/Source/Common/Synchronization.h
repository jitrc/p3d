#pragma once

#if defined(P3D_WINDOWS)
#include "Windows/Synchronization.h"
#include "Windows/Thread.h"
#endif

// Synchronization primitives

namespace P3D
{
    typedef Implementation::Event::EventHandle EventHandle;
    typedef Implementation::Thread::ThreadID   ThreadID;

    /*
    Synchronization object that provides event notification.
    */
    class Event
    {
        typedef Implementation::Event Impl;
    public:
        enum EventFlags
        {
            AutoReset           = 1 << 0,
            SetSignaled         = 1 << 1,
            DelayInitialization = 1 << 2
        };

        enum WaitResult
        {
            Signaled = 0,
            Timeout  = 1
        };

        inline explicit Event(int flags = DelayInitialization)
        {
            _handle = NULL;
            if ((flags & DelayInitialization) == 0)
                Initialize(flags);
        }

        inline void Initialize(int flags)
        {
            ASSERT(_handle == NULL);
            ASSERT((flags & DelayInitialization) == 0);
            _handle = Impl::CreateEventHandle((flags & AutoReset) != 0, (flags & SetSignaled) != 0);
        }

        inline ~Event()
        {
            if (_handle)
                Impl::DestroyEventHandle(_handle);
        }

        inline WaitResult Wait(uint timeout = INFINITE) const
        {
            // Returns true on timeout
            if (Impl::WaitOne(_handle, timeout))
                return WaitResult::Timeout;
            else
                return WaitResult::Signaled;
        }

        inline void Signal()
        {
            Impl::Signal(_handle);
        }

        inline void Clear()
        {
            Impl::Clear(_handle);
        }

        inline EventHandle GetHandle() const
        {
            return _handle;
        }

        /*
        Wait for any event from the groups of events.
        Return -1 on timeout and index of the first signaled event otherwise.
        */
        static inline int WaitAny(Event** events, uint count, uint timeout = INFINITE)
        {
            ASSERT(events != NULL);
            ASSERT(count < Impl::MaxWaitHandles);

            EventHandle handles[Impl::MaxWaitHandles];
            for (uint i = 0; i < count; i++)
                handles[i] = events[i]->_handle;
            return Impl::WaitAny(handles, count, timeout);
        }

        /*
        Shorter version for just two events.
        */
        static inline int WaitAny(Event& e0, Event& e1, uint timeout = INFINITE)
        {
            EventHandle handles[2] = {e0._handle, e1._handle};
            return Impl::WaitAny(handles, 2, timeout);
        }

        /*
        Wait for any event from the groups of events.
        Return -1 on timeout and index of the first signaled event otherwise.
        */
        static inline int WaitAll(Event** events, uint count, uint timeout = INFINITE)
        {
            ASSERT(events != NULL);
            ASSERT(count < Impl::MaxWaitHandles);

            EventHandle handles[Impl::MaxWaitHandles];
            for (uint i = 0; i < count; i++)
                handles[i] = events[i]->_handle;
            return Impl::WaitAll(handles, count, timeout);
        }

        /*
        Shorter version for just two events.
        */
        static inline int WaitAll(Event& e0, Event& e1, uint timeout = INFINITE)
        {
            EventHandle handles[2] = {e0._handle, e1._handle};
            return Impl::WaitAll(handles, 2, timeout);
        }

    private:
        mutable EventHandle _handle;
    };

    class ManualEvent : public Event
    {
    public:
        inline explicit ManualEvent() : Event(0)
        {}
    };

    /*
    Guards block of code to be executed only in one thread at a time.
    */
    class Lock
    {
        typedef Implementation::Lock Impl;
    public:
        inline Lock(int spinCount = 4000)
        {
            Impl::Initialize(&_cs, spinCount);
        #ifdef _DEBUG
            _ownerId = 0;
        #endif
        }

        inline ~Lock()
        {
            Impl::Free(&_cs);
        }

        inline void Enter()
        {
            Impl::Enter(&_cs);
            #ifdef _DEBUG
            _ownerId = Implementation::Thread::GetCurrentThreadID();
            #endif
        }

        inline void Leave() 
        { 
            #ifdef _DEBUG
            _ownerId = 0;
            #endif
            Impl::Leave(&_cs);
        }

        inline bool TryEnter()
        {
            return Impl::TryEnter(&_cs);
        }

    private:
    #ifdef _DEBUG
        ThreadID _ownerId; // ID of the thread that locked the section
    #endif
        Impl::DataType _cs;
    };

    /*
    Mixin object that owns lock.
    */
    template<class LockType = Lock>
    class ObjectWithLock
    {
    public:
        LockType& GetLock() { return _lock; }

    private:
        LockType _lock;
    };

    /*
    Locks critical section in the constructor and unlocks it in the desctructor.
    */
    template<class Lockable>
    class AutoLockTemplate
    {
        Lockable* critSectionPtr;
    public:
        AutoLockTemplate(Lockable& handle)
        {
            critSectionPtr = &handle;
            ASSERT(critSectionPtr != NULL);
            critSectionPtr->Enter();
        }

        AutoLockTemplate(ObjectWithLock<Lockable>* object)
        {
            critSectionPtr = &object->GetLock();
            ASSERT(critSectionPtr != NULL);
            critSectionPtr->Enter();
        }

        AutoLockTemplate(ObjectWithLock<Lockable>& object)
        {
            critSectionPtr = &object.GetLock();
            ASSERT(critSectionPtr != NULL);
            critSectionPtr->Enter();
        }

        ~AutoLockTemplate()
        {
            critSectionPtr->Leave();
        }

        // required to implement auto_lock(cs)
        operator int() const { return 0; }
    };

    typedef AutoLockTemplate<Lock> AutoLock;

    // Neat define for AutoLock. Usage:
    // auto_lock (lock) { ... } <=> { AutoLock locker(lock); ... }
    #define auto_lock(cs) if (::P3D::AutoLock __lock__ = ::P3D::AutoLock((cs))) { UNREACHABLE(); } else 
}