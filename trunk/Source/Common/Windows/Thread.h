#pragma once

#ifndef P3D_WINDOWS
#error The file should be included under Windows only.
#endif

namespace P3D
{
    namespace Implementation
    {
        class Thread
        {
        public:
            typedef DWORD  ThreadID;
            typedef HANDLE ThreadHandle;
            typedef int (*ThreadProc)(void* param1, void* param2);
            typedef DWORD TLSIndex;

            static ThreadID GetCurrentThreadID()
            {
                return ::GetCurrentThreadId();
            }
            static bool RunThread(ThreadHandle* handle, ThreadID* id, ThreadProc proc, void* param1, void* param2)
            {
                ASSERT(handle != NULL);
                ASSERT(id != NULL);
                ASSERT(proc != NULL);
                StartupParams* params = new StartupParams(proc, param1, param2);
                *handle = CreateThread(NULL, 0, _Run, params, 0, id);
                if (*handle == NULL)
                {
                    *id = 0;
                    delete params;
                    return false;
                } else
                    return true;
            }
            static void DestroyThreadHandle(ThreadHandle handle)
            {
                CloseHandle(handle);
            }
            static bool Join(ThreadHandle handle, uint timeout)
            {
                return WaitForSingleObject(handle, timeout) != WAIT_TIMEOUT;
            }
            static TLSIndex AllocTLSIndex()
            {
                DWORD index = TlsAlloc();
                if (index != TLS_OUT_OF_INDEXES)
                    return index;
                else
                    return -1;
            }
            static void SetTLSValue(TLSIndex index, void* value)
            {
                TlsSetValue(index, value);
            }
            static void* GetTLSValue(TLSIndex index)
            {
                return TlsGetValue(index);
            }

        private:
            struct StartupParams
            {
                StartupParams(ThreadProc p, void* p1, void* p2) : proc(p), param1(p1), param2(p2) {}

                ThreadProc proc;
                void* param1;
                void* param2;
            };

            static DWORD WINAPI _Run(LPVOID param)
            {
                StartupParams* params = static_cast<StartupParams*>(param);
                ThreadProc proc = params->proc;
                void* tp1 = params->param1;
                void* tp2 = params->param2;
                delete params;
                return proc(tp1, tp2);
            }
        };
    }
}