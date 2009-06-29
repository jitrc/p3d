#pragma once

#ifndef P3D_WINDOWS
#error The file should be included under Windows only.
#endif

namespace P3D
{
    namespace Implementation
    {
        class Event
        {
        public:
            typedef HANDLE EventHandle;
            static const int MaxWaitHandles = 64;

            inline static EventHandle CreateEventHandle(bool autoreset, bool initialState)
            {
                return ::CreateEventW(NULL, !autoreset, initialState, NULL);
            }
            inline static void DestroyEventHandle(EventHandle handle)
            {
                ASSERT(handle != NULL);
                ::CloseHandle(handle);
            }
            inline static void Signal(EventHandle handle)
            {
                ASSERT(handle != NULL);
                ::SetEvent(handle);
            }
            inline static void Clear(EventHandle handle)
            {
                ASSERT(handle != NULL);
                ::ResetEvent(handle);
            }
            inline static bool WaitOne(EventHandle handle, uint timeout)
            {
                ASSERT(handle != NULL);
                return (WAIT_TIMEOUT == WaitForSingleObject(handle, timeout));
            }
            inline static int WaitN(EventHandle* handles, uint count, uint timeout, bool waitAll)
            {
                DWORD res = WaitForMultipleObjects(count, handles, waitAll, timeout);
                if (res == WAIT_TIMEOUT)
                    return -1;
                else
                    return res - WAIT_OBJECT_0;
            }
            inline static int WaitAny(EventHandle* handles, uint count, uint timeout)
            {
                return WaitN(handles, count, timeout, false);
            }
            inline static int WaitAll(EventHandle* handles, uint count, uint timeout)
            {
                return WaitN(handles, count, timeout, true);
            }
        };

        class Lock
        {
        public:
            typedef CRITICAL_SECTION DataType;
            static void Initialize(DataType* cs, int spinCount)
            {
                memset(cs, 0, sizeof(DataType));
                if (spinCount == 0)
                    InitializeCriticalSection(cs);
                else
                    InitializeCriticalSectionAndSpinCount(cs, spinCount);
            }
            static void Free(DataType* cs)
            {
                DeleteCriticalSection(cs);
            }
            static void Enter(DataType* cs)
            {
                EnterCriticalSection(cs); 
            }
            static void Leave(DataType* cs)
            {
                LeaveCriticalSection(cs);
            }
            static bool TryEnter(DataType* cs)
            {
                return TryEnterCriticalSection(cs) != FALSE;
            }
        };
    }
}