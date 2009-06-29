#pragma once

#include "LogMessage.h"
#include "Object.h"

namespace P3D
{
    /*
    Accepts log messages from the Log class.
    Abstract class.
    */    
    class LogAppender 
        : public Object
    {
    public:
        /*
        Called by Log to process new log message.
        */
        virtual void ProcessLogMessage(const LogMessage& msg) = 0;
    };
}