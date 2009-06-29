#include "Includes.h"
#include "AtExitManager.h"

namespace P3D
{
    Lock AtExitManager::_lock;
    AtExitManager* AtExitManager::_instance = NULL;

    AtExitManager::AtExitManager()
    {
        AutoLock lock(_lock);
        ASSERT(_instance == NULL);
        _instance = this;
        _triggeringNow = false;
    }

    AtExitManager::~AtExitManager()
    {
        AutoLock lock(_lock);
        TriggerCallbacks();
        _instance = NULL;
    }

    void AtExitManager::Register(AtExitCallback callback)
    {
        if (_triggeringNow || callback == NULL) return;
        AutoLock lock(_lock);
        _stack.push(callback);
    }

    void AtExitManager::TriggerCallbacks()
    {
        AutoLock lock(_lock);
        _triggeringNow = true;
        while (!_stack.empty())
        {
            try
            {
                AtExitCallback callback = _stack.top();
                _stack.pop();
                callback();
            }
            catch (...)
            {
                // skip all exceptions
                // just try to finish the job
                // it would be nice to log it somewhere, but logger probably already dead
            }
        }
        _triggeringNow = false;
    }
}