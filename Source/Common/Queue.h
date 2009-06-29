#pragma once

#include "Object.h"
#include "Synchronization.h"
#include "TemplateTricks.h"

namespace P3D
{
    /*
    Thread-safe queue of pointers to T.
    TODO: make lock-free
    */
    template<class T>
    class Queue 
    {
    public:
        Queue() : _notEmpty(0), _shutdownEvent(0), _shutdown(false)
        { }

        ~Queue()
        {
            Shutdown();
        }

        // Producer thread methods

        /*
        Adds message to the queue.
        The method does not AddRefs it but take ownership.
        */
        void Put(T* msg)
        {
            auto_lock(_lock)
            {
                if (_shutdown)
                {
                    ReleaseIfNeeded(msg);
                    return;
                }
                _queue.push_back(msg);
                _notEmpty.Signal();
            }
        }

        /*
        Unblocks all GetMessage methods and clear message queue (if any).
        */
        void Shutdown()
        {
            auto_lock(_lock)
            {
                _shutdown = true;
                _shutdownEvent.Signal();

                for (std::list<T*>::iterator it = _queue.begin(); it != _queue.end(); ++it)
                    ReleaseIfNeeded(*it);
                _queue.clear();
            }
        }

        /*
        Return instance to the working state after Shutdown call.
        */
        void Reinitialize()
        {
            auto_lock(_lock)
            {
                if (_shutdown)
                {
                    _notEmpty.Clear();
                    _shutdownEvent.Clear();
                    _shutdown = false;
                }
            }
        }

        // Consumer thread methods

        /*
        Return next message in the queue or NULL if the queue is empty.
        */
        T* Peek()
        {
            auto_lock(_lock)
            {
                if (_queue.empty())
                    return NULL;
                else
                {
                    T* front = _queue.front();
                    _queue.pop_front();
                    if (_queue.empty())
                        _notEmpty.Clear();
                    return front;
                }
            }
        }

        /*
        Wait for message and return it. Return NULL in case of timeout.
        */
        T* Get(int timeout = INFINITE)
        {
            clock_t start = clock();
            while (true)
            {
                T* msg = Peek();
                if (msg) return msg;
                if (_shutdown) return NULL;
                if (Event::WaitAny(_notEmpty, _shutdownEvent, timeout) != 0)
                    return NULL;
                if (timeout != INFINITE)
                {
                    if (clock() - start > timeout * CLOCKS_PER_SEC / 1000)
                        return NULL;
                }
            }
        }

        /*
        Return the length of the queue.
        Not safe.
        */
        int GetLength() const
        {
            auto_lock(_lock)
            {
                return _queue.size();
            }
        }

        /*
        Return true in case queue is about to shutdown.
        */
        bool IsShuttingDown() const
        {
            return _shutdown;
        }

        /*
        Event that is signaled only in case the queue is not empty.
        Should be used only for waiting on!
        */
        const Event& GetNotEmptyEvent() const { return _notEmpty; }

        /*
        Event that is singled when queue should die.
        Should be used only for waiting on!
        */
        const Event& GetShutdownEvent() const { return _shutdownEvent; }

    private:
        std::list<T*> _queue;
        mutable Lock _lock;

        Event _notEmpty;
        Event _shutdownEvent;
        bool _shutdown;
    };
}