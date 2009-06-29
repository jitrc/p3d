#pragma once

#include "Synchronization.h"

namespace P3D
{
    /*
    Allows registration of callbacks that should be called during normal program shutdown.
    Using such manager allows to control the order of operations.
    Instantiate it on the stack somewhere at the begining of the int main(...) and in the desctructor 
    all callbacks would be called in LIFO order.
    Only one instance is allowed. In other case assertion will be triggered.
    */
    class AtExitManager
    {
    public:
        AtExitManager();
        ~AtExitManager();

        typedef void (*AtExitCallback)();

        /*
        Register new at exit callback.
        */
        static void RegisterAtExit(AtExitCallback callback) 
        { 
            if (_instance) 
                _instance->Register(callback);
            else
                atexit(callback); // fallback to CRT
        }

    private:
        static AtExitManager* _instance;
        static Lock _lock;

        void Register(AtExitCallback callback);
        void TriggerCallbacks();

        typedef std::stack<AtExitCallback> CallbacksStack;
        CallbacksStack _stack;
        bool _triggeringNow;
    };
}
