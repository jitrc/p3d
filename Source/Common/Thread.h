#pragma once

#include "Exceptions.h"
#include "Synchronization.h"
#include "Logger.h"
#include "Command.h"
#include "Queue.h"
#include "Atomic.h"
#include "ObjectWithTags.h"
#include "Singleton.h"
#include "Observable.h"

#if defined(P3D_WINDOWS)
#include "Windows\Thread.h"
#endif

namespace P3D
{
    typedef Implementation::Thread::ThreadID ThreadID;

    class MainThread;

    /*
    Single thread of execution.
    Manages list of Command objects that want to be executed in this thread.
    */
    class Thread :
        public Object,
        public ExecutionContext,
        public ObjectWithTags
    {
    protected:
        Logger logger;

        // constructor for MainThread
        Thread(bool main);

    public:
        Thread();
        virtual ~Thread();

        /*
        Start execution of commands.
        */
        void Run(const char* name);

        /*
        Reject all incoming commands, execute ones left in queue and stop thread.
        */
        void Join();
        void Stop() { Join(); }

        /*
        Is thread running?
        */
        bool IsRunning() { return _id != 0; }

        /*
        This thread ID.
        */
        ThreadID GetThreadID() const { return (ThreadID)_id; }

        /*
        Name of this thread as passed in constructor.
        */
        const std::string& GetName() const { return _name; }

        /*
        Is this thread currently executing?
        */
        bool IsExecutingThread()
        {
            return (_id != 0) && (GetCurrentThreadID() == _id);
        }

        // From ExecutionContext

        /*
        Appends command to execution list of the thread.
        */
        override void Invoke(Command* command);

        // Static methods

        /*
        ID of the currently running thread.
        */
        static ThreadID GetCurrentThreadID()
        {
            return Impl::GetCurrentThreadID();
        }

        /*
        Pointer to the currently running Thread instance.
        */
        static Thread* GetCurrentThread()
        {
            return static_cast<Thread*>(Impl::GetTLSValue(_tlsIndex));
        }

        /*
        Pointer to the main thread.
        */
        static MainThread* GetMainThread()
        {
            return _mainThread;
        }

    protected:
        virtual void Initialize();
        virtual void Deinitialize();
        virtual void MessageLoop();
        virtual void MarkForTermination();

        Fenced<bool> _stoppingNow;

    private:
        typedef Implementation::Thread Impl;

        int _Thread();

        static int _Run(void* thiz, void* dummy)
        {
            return static_cast<Thread*>(thiz)->_Thread();
        }

        Fenced<ThreadID> _id;
        Impl::ThreadHandle _handle;
        std::string _name;
        Lock _lock;
        Queue<Command> _queue;
        Event _startedEvent;
        const bool _isMainThread;
        
        static Impl::TLSIndex _tlsIndex;
        static MainThread* _mainThread;
    };

    /*
    Throw this from within thread to terminate it.
    */
    class TerminateThreadException : public Exception
    { 
    public:
        TerminateThreadException() : Exception(L"Thread termination.") 
        { }
    };

    /*
    Listens to the event from main thread.
    */
    class MainThreadListener
    {
    public:
        /*
        Before main loop.
        */
        virtual void OnInitialize() {}

        /*
        After main loop.
        */
        virtual void OnDeinitialize() {}

        /*
        Window has been resized.
        */
        virtual void OnResize(int width, int height) {}

        /*
        On each iteration of main loop.
        */
        virtual void OnIdle() {}

        /*
        On key pressed.
        */
        virtual void OnKeyDown(SDL_keysym key) {}

        /*
        On key released.
        */
        virtual void OnKeyUp(SDL_keysym key) {}

        /*
        On mouse button pressed.
        */
        virtual void OnMouseDown(uint buttonIndex, int mx, int my) {}

        /*
        On key released.
        */
        virtual void OnMouseUp(uint buttonIndex, int mx, int my) {}

        /*
        Mouse moves.
        */
        virtual void OnMouseMove(int dx, int dy, int mx, int my) {}
    };

    /*
    Incapsulate main message loop.
    Singleton object. Use Thread::GetMainThread to access it.
    */
    class MainThread : 
        public Thread,
        public Observable<MainThreadListener>
    {
    protected:
        MainThread() : Thread(true), _delay(0) {}

    public:
        /*
        Sets delay between iterations in the main loop.
        Use -1 for blocking mode.
        Use 0 for busy loop.
        */
        void SetMainLoopDelay(uint delay) { _delay = delay; }

    protected:
        uint _delay;
    };

}