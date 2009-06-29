#include "Includes.h"
#include "Command.h"
#include "Thread.h"

namespace P3D
{
    namespace Internal
    {
        ExecutionContext* g_MainThreadContext = NULL;
        ExecutionContext* g_ThreadPoolContext = NULL;
    }

    void SetSpecialContext(ExecutionContext* specialContext, ExecutionContext* value)
    {
        if (specialContext == CONTEXT_MAIN_THREAD) Internal::g_MainThreadContext = value;
        if (specialContext == CONTEXT_THREAD_POOL) Internal::g_ThreadPoolContext = value;
    }

    ExecutionContext* ResolveContext(ExecutionContext* context)
    {
        if (context == CONTEXT_MAIN_THREAD) return Internal::g_MainThreadContext;
        if (context == CONTEXT_THREAD_POOL) return Internal::g_ThreadPoolContext;
        if (context == CONTEXT_CURRENT_THREAD)
            return Thread::GetCurrentThread();
        return context;
    }
}