#pragma once

#include "LoggingSystem.h"
#include "LogStream.h"

namespace P3D
{
    /*
    Auto append source to the log message.
    */
    class Logger
    {
    public:
        inline Logger(const String& source) : Source(source) { }
        inline ~Logger() { }
        inline void Message(LogLevel level, const wchar* msg) { LoggingSystem::GetInstance().Message(msg, level, Source); }

        inline LogStream log(LogLevel level) { return LogStream(this, level); }
        inline LogStream trace() { return log(LOG_TRACE); }
        inline LogStream debug() { return log(LOG_DEBUG); }
        inline LogStream info() { return log(LOG_INFO); }
        inline LogStream warn() { return log(LOG_WARN); }
        inline LogStream error() { return log(LOG_ERROR); }

    public:
        String Source;
    };
}