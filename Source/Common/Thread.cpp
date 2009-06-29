#include "Includes.h"
#include "Thread.h"

namespace P3D
{
    const int SDL_INVOKE_EVENT = SDL_USEREVENT + 6;

#define CallEventHandler(...) {\
    try { DispatchEvent(__VA_ARGS__); }\
    catch (TerminateThreadException&) { MarkForTermination(); }\
    catch (Exception& exc) { logger.error() << L"Unhandled exception: " << exc; }\
    catch (std::exception& exc) { logger.error() << L"Unhandled exception: " << exc.what(); }\
    catch (...) { logger.error() << L"Unknown unhandled exception."; }\
    }

    class MainThreadImpl : 
        public MainThread
    {
    public:
        MainThreadImpl()
        { }

        /*
        Appends command to execution list of the thread.
        For main thread we use SDL event queue to notify message loop 
        about new command.
        */
        override void Invoke(Command* command)
        {
            if (!_stoppingNow.Unfenced())
            {
                SDL_Event event;
                event.type = SDL_INVOKE_EVENT;
                event.user.code = 0;
                event.user.data1 = command;
                event.user.data2 = NULL;
                if (SDL_PushEvent(&event) != 0)
                {
                    logger.warn() << L"Failed to invoke command in the main thread. "
                        <<  "Perhapse event queue is full.";
                }
            } else
            {
                logger.info() << L"Thread is stopping, rejecting command.";
                command->Release();
            }
        }

    protected:
        override void Initialize()
        {
            Thread::Initialize();
            CallEventHandler(&MainThreadListener::OnInitialize);
        }

        override void Deinitialize()
        {
            CallEventHandler(&MainThreadListener::OnDeinitialize);
            Thread::Deinitialize();
        }

        override void MarkForTermination()
        {
            if (!_stoppingNow)
            {
                _stoppingNow = true;

                // use '-1' as "stop now marker"
                SDL_Event event;
                event.type = SDL_INVOKE_EVENT;
                event.user.code = 0;
                event.user.data1 = (void*)-1;
                event.user.data2 = NULL;
                if (SDL_PushEvent(&event) != 0)
                {
                    logger.warn() << L"Failed to invoke command in the main thread. "
                        <<  "Perhapse event queue is full.";
                }
            }
        }

        bool ProcessInvokeEvent(SDL_Event& event)
        {
            Command* msg = (Command*)event.user.data1;
            if (msg == (Command*)-1)
                return false; // stop now
            try
            {
                msg->Execute(this);
            }
            catch (TerminateThreadException&)
            {
                MarkForTermination();
            }
            catch (Exception& exc)
            {
                logger.error() << L"Unhandled exception: " << exc;
            }
            catch (std::exception& exc)
            {
                logger.error() << L"Unhandled exception: " << exc.what();
            }
            catch (...)
            {
                logger.error() << L"Unknown unhandled exception.";
            }
            msg->Release();
            return true;
        }

        bool ProcessEvent(SDL_Event& event)
        {
            switch (event.type)
            {
                case SDL_INVOKE_EVENT:
                    if (!ProcessInvokeEvent(event))
                        return false;
                    break;
                case SDL_QUIT:
                    MarkForTermination();
                    break;
                case SDL_KEYDOWN:
                    CallEventHandler(&MainThreadListener::OnKeyDown, event.key.keysym);
                    break;
                case SDL_KEYUP:
                    CallEventHandler(&MainThreadListener::OnKeyUp, event.key.keysym);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    CallEventHandler(&MainThreadListener::OnMouseDown, 
                        event.button.button, event.button.x, event.button.x);
                    break;
                case SDL_MOUSEBUTTONUP:
                    CallEventHandler(&MainThreadListener::OnMouseUp, 
                        event.button.button, event.button.x, event.button.x);
                    break;
                case SDL_MOUSEMOTION:
                    CallEventHandler(&MainThreadListener::OnMouseMove, 
                        event.motion.xrel, event.motion.yrel, 
                        event.motion.x, event.motion.y);
                    break;
                case SDL_VIDEORESIZE:
                    CallEventHandler(&MainThreadListener::OnResize, 
                        event.resize.w, event.resize.h);
                    break;
            }
            return true;
        }

        override void MessageLoop()
        {
            int errCount = 0;
            while (true)
            {
                SDL_Event event;
                CallEventHandler(&MainThreadListener::OnIdle);
                if (_delay == -1)
                {
                    int res = SDL_WaitEvent(&event);
                    if (!res)
                    {
                        logger.warn() << L"SDL_WaitEvent failed: " << ToUTF16(SDL_GetError());
                        errCount++;
                        if (errCount > 20)
                        {
                            logger.error() << L"Too much errors in SDL_WaitEvent. Something broke. Quiting main thread.";
                            return;
                        } else
                            SDL_Delay(20);
                    } else
                    {
                        errCount = 0;
                        if (!ProcessEvent(event)) return;
                        while (SDL_PollEvent(&event))
                        {
                            if (!ProcessEvent(event))
                                return;
                        }
                    }
                } else
                {
                    while (SDL_PollEvent(&event))
                    {
                        if (!ProcessEvent(event))
                            return;
                    }
                    if (_delay) SDL_Delay(_delay);
                }
            }
        }
    };

    MainThreadImpl g_MainThread;

    Thread::Impl::TLSIndex Thread::_tlsIndex;
    MainThread* Thread::_mainThread = NULL;

    Thread::Thread(bool main) 
        : _name("MainThread"), _id(0), logger(L"System.Thread.MainThread"), _startedEvent(Event::AutoReset), _stoppingNow(false),
        _isMainThread(true)
    {
        ASSERT(_mainThread == NULL);
        ASSERT(_tlsIndex == 0);

        _mainThread = (MainThread*)this;
        _id = GetCurrentThreadID();
        _handle = NULL;
        _tlsIndex = Thread::Impl::AllocTLSIndex();
        Impl::SetTLSValue(_tlsIndex, this);
        SetSpecialContext(CONTEXT_MAIN_THREAD, this);
    }

    Thread::Thread() 
        : _name("Unnamed"), logger(L"System.Thread.Unnamed"),
        _id(0), _startedEvent(Event::AutoReset), _stoppingNow(false),
        _isMainThread(false)
    {
    }

    Thread::~Thread()
    {
        ASSERT(_isMainThread || (!_isMainThread && !IsRunning()));
    }

    void Thread::Run(const char* name)
    {
        ASSERT(_isMainThread || (!_isMainThread && !IsRunning()));

        _name = name;
        logger.Source = L"System.Thread." + ToUTF16(_name);

        _queue.Reinitialize();
        if (!_isMainThread)
        {
            if (!Impl::RunThread(&_handle, &_id.Unfenced(), _Run, this, NULL))
            {
                logger.info() << L"Failed to start the thread!";
                abort();
            } else
                _startedEvent.Wait();
        } else
        {
            // its true if called from main thread
            ASSERT(GetCurrentThread() == this); 
            _Thread();
        }
    }

    void Thread::MarkForTermination()
    {
        if (!_stoppingNow)
        {
            _stoppingNow = true;
            _queue.Put((Command*)-1); // use '-1' as "stop now marker"
        }
    }

    void Thread::Join()
    {
        if (IsExecutingThread())
        {
            logger.info() << L"Stoping thread from its own context...";
            throw TerminateThreadException();
        } else
        {
            logger.info() << L"Stopping thread from another context...";
            MarkForTermination();
            Impl::Join(_handle, INFINITE);
        }
    }

    void Thread::Invoke(Command* command)
    {
        if (!_stoppingNow.Unfenced())
            _queue.Put(command);
        else
        {
            logger.info() << L"Thread is stopping, rejecting command.";
            command->Release();
        }
    }

    int Thread::_Thread()
    {
        Initialize();
        _startedEvent.Signal();
        MessageLoop();
        Deinitialize();
        return 0;
    }

    void Thread::Initialize()
    {
        AddRef();
        Impl::SetTLSValue(_tlsIndex, this);
    }

    void Thread::Deinitialize()
    {
        _id = 0;
        if (_handle)
            Impl::DestroyThreadHandle(_handle);
        _handle = 0;
        _stoppingNow = false;
        Impl::SetTLSValue(_tlsIndex, NULL);
        Release();
    }

    void Thread::MessageLoop()
    {
        while (true)
        {
            Command* msg = _queue.Get();
            if (msg == NULL && _queue.IsShuttingDown())
                break;
            if (msg == (Command*)-1)
            {
                _queue.Shutdown();
                break;
            }
            try
            {
                msg->Execute(this);
            }
            catch (TerminateThreadException&)
            {
                MarkForTermination();
            }
            catch (Exception& exc)
            {
                logger.error() << L"Unhandled exception: " << exc;
            }
            catch (std::exception& exc)
            {
                logger.error() << L"Unhandled exception: " << exc.what();
            }
            catch (...)
            {
                logger.error() << L"Unknown unhandled exception.";
            }
            msg->Release();
        }
    }
}