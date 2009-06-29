#pragma once

#include "Command.h"
#include "StdCommands.h"
#include "Synchronization.h"
#include "TemplateTricks.h"

namespace P3D
{
    // Possible value for ThreadSafePolicy template parameter in Observer<>
    struct ThreadSafe
    { };

    // Possible value for ThreadSafePolicy template parameter in Observer<>
    struct NotThreadSafe
    { };

    namespace Internal
    {
        template<class T>
        class ThreadSafetyPolicy;

        template<>
        class ThreadSafetyPolicy<ThreadSafe>
        {
        public:
            typedef AutoLock AutoLockType; 

            void Enter() { _lock.Enter(); }
            void Leave() { _lock.Leave(); }
            Lock& GetLockable() {  return _lock; }
        private:
            Lock _lock;
        };

        template<>
        class ThreadSafetyPolicy<NotThreadSafe>
        {
        public:
            typedef int AutoLockType;

            void Enter() { }
            void Leave() { }
            int GetLockable() { return 0; }
        };
    }

    /*
    Base template class for event dispatcher.
    Usage:
    class MyObservable : public Observable<MyListener, ThreadSafe>
    {
    ....
        void method()
        {
           DispatchEvent(&MyListener::HandleEvent, arg1, arg2);
        }
    ...
    };
    */
    template
    <
        class ListenerType,  // type of the listeners
        class ThreadSafePolicy = ThreadSafe // surround code with lock? Possible values are ThreadSafe and NotThreadSafe
    >
    class Observable
    {
    public:
        /*
        Add listener to the list.
        Adding one listener twice is not allower.
        */
        void AddListener(ListenerType* listener, ExecutionContext* executionContext = CONTEXT_CALL_NOW)
        {
            MTPolicy::AutoLockType autolock(_mt.GetLockable());
            AddRefIfNeeded(listener);
            _listeners.push_back(std::make_pair(listener, ResolveContext(executionContext)));
        }

        /*
        Remove listener from the list.
        */
        void RemoveListener(ListenerType* listener)
        {
            MTPolicy::AutoLockType autolock(_mt.GetLockable());
            for (ListenersVector::iterator it = _listeners.begin(); it != _listeners.end(); ++it)
            {
                if (it->first == listener)
                {
                    _listeners.erase(it);
                    ReleaseIfNeeded(listener);
                    return;
                }
            }
        }

        /*
        Remove all listeners.
        */
        void RemoveAllListeners()
        {
            MTPolicy::AutoLockType autolock(_mt.GetLockable());
            for (ListenersVector::iterator it = _listeners.begin(); it != _listeners.end(); ++it)
                ReleaseIfNeeded(it->first);
            _listeners.clear();
        }

        /*
        Return true in case there is a least one listener.
        */
        bool HasListeners() const
        {
            MTPolicy::AutoLockType autolock(_mt.GetLockable());
            return !_listeners.empty();
        }

    protected:
#define DISPATCH_IMPL(...) \
        {\
            ListenersVector listenersCopy; \
            { \
                MTPolicy::AutoLockType autolock(_mt.GetLockable()); \
                listenersCopy = _listeners; \
            } \
            for (ListenersVector::iterator it = listenersCopy.begin(); it != listenersCopy.end(); ++it) \
            {\
                if (it->second == NULL)\
                    ((*(it->first)).*(method))(__VA_ARGS__);\
                else\
                    it->second->Invoke(MC(it->first, method, __VA_ARGS__));\
            }\
        }

        template<class M>
        void DispatchEvent(M method)
        DISPATCH_IMPL()

        template<class M, class Arg1>
        void DispatchEvent(M method, Arg1 a1)
        DISPATCH_IMPL(a1)

        template<class M, class Arg1, class Arg2>
        void DispatchEvent(M method, Arg1 a1, Arg2 a2)
        DISPATCH_IMPL(a1, a2)

        template<class M, class Arg1, class Arg2, class Arg3>
        void DispatchEvent(M method, Arg1 a1, Arg2 a2, Arg3 a3)
        DISPATCH_IMPL(a1, a2, a3)

        template<class M, class Arg1, class Arg2, class Arg3, class Arg4>
        void DispatchEvent(M method, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4)
        DISPATCH_IMPL(a1, a2, a3, a4)

#undef DISPATCH_IMPL

    private:
        typedef std::vector<std::pair<ListenerType*, ExecutionContext*> > ListenersVector;
        ListenersVector _listeners;

        typedef Internal::ThreadSafetyPolicy<ThreadSafePolicy> MTPolicy;
        mutable MTPolicy _mt;
    };
}