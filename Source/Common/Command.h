#pragma once

#include "Logger.h"
#include "Object.h"
#include "Boxed.h"

namespace P3D
{
    class Command;
    class ExecutionContext;

    /*
    Command that can be invoked.
    */
    class Command 
        : public Object
    {
    public:
        virtual void Execute(ExecutionContext* context) = 0;
    };

    /*
    Object that can execute commands.
    */
    class ExecutionContext
    {
    public:
        enum InvokationResult
        {
            Executed,
            Timeout,
            Stopped
        };

        /*
        Schedules command for execution (or executes immediatly).
        Releases the command.
        */
        virtual void Invoke(Command* command) = 0;

        /*
        Invokes command and return Boxed<ManualEvent> that will be in signaled state when command finish execution.
        It should be released manually.
        */
        Boxed<ManualEvent>* InvokeOverlapped(Command* command)
        {
            ASSERT(command != NULL);
            Boxed<ManualEvent>* waitEvent = CreateObjectInPool<Boxed<ManualEvent> >();
            Invoke(new ExecuteAndSignalEvent(command, waitEvent));
            return waitEvent;
        }

        /*
        Invokes command and waits till it is finished.
        */
        InvokationResult InvokeAndWait(Command* command, uint timeout = INFINITE, Event* stopEvent = NULL)
        {
            ASSERT(command != NULL);

            Boxed<ManualEvent>* waitEvent = InvokeOverlapped(command);
            InvokationResult res;
            if (stopEvent)
            {
                switch (Event::WaitAny(waitEvent->Unbox(), *stopEvent, timeout))
                {
                    case -1: res = Timeout; break;
                    case  0: res = Executed; break;
                    case  1: res = Stopped; break;
                }
            } else
            {
                if (waitEvent->Unbox().Wait(timeout))
                    res = Timeout;
                else
                    res = Executed;
            }
            waitEvent->Release();
            return res;
        }

    private:
        class ExecuteAndSignalEvent : 
            public Command
        {
        public:
            ExecuteAndSignalEvent(Command* cmd, Boxed<ManualEvent>* signal) : _cmd(cmd), _signal(signal)
            {
                _cmd->AddRef();
                _signal->AddRef();
            }
            virtual ~ExecuteAndSignalEvent()
            {
                _signal->Release();
                _cmd->Release();
            }
            override void Execute(ExecutionContext* cmdHandler)
            {
                _cmd->Execute(cmdHandler);
                _signal->Unbox().Signal();
            }
        private:
            Command* _cmd;
            Boxed<ManualEvent>* _signal;
        };
    };

    static ExecutionContext* CONTEXT_CALL_NOW       = 0;
    static ExecutionContext* CONTEXT_THREAD_POOL    = reinterpret_cast<ExecutionContext*>(-1);
    static ExecutionContext* CONTEXT_MAIN_THREAD    = reinterpret_cast<ExecutionContext*>(-2);
    static ExecutionContext* CONTEXT_CURRENT_THREAD = reinterpret_cast<ExecutionContext*>(-3);

    /*
    Sets real execution contexts that will be used insteed of CONTEXT_THREAD_POOL and CONTEXT_UI.
    */
    void SetSpecialContext(ExecutionContext* specialContext, ExecutionContext* value);

    /*
    Return the real context behind special one.
    */
    ExecutionContext* ResolveContext(ExecutionContext* context);

    /*
    Invokes command in specified context resolving it before the call.
    */
    static inline void Invoke(Command* cmd, ExecutionContext* context)
    {
        if (context == CONTEXT_CALL_NOW)
        {
            cmd->Execute(CONTEXT_CALL_NOW);
            cmd->Release();
        } else
        {
            ExecutionContext* ctx = ResolveContext(context);
            if (ctx) 
                ctx->Invoke(cmd);
            else
            {
                LoggingSystem::GetInstance().Message(L"Special context is not defined!", LOG_ERROR, L"System.Invoke");
                cmd->Release();
            }
        }
    }
}