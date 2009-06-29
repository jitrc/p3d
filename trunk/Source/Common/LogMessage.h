#pragma once

namespace P3D
{
    /*
    Possible log levels.
    */
    enum LogLevel
    {
        LOG_TRACE = 1,
        LOG_DEBUG,
        LOG_INFO,
        LOG_WARN,
        LOG_ERROR
    };

    /*
    Represent log message passed by reference between Log and LogAppenders.
    */
    class LogMessage
    {
    public:
        const wchar* Message;
        LogLevel Level;
        const wchar* Source;
        const wchar* Time;
        long ThreadID;

        LogMessage(const wchar* msg, LogLevel level, const wchar* source, const wchar* time);

        /*
        Formats message in default way:
        [LEVEL] [TIME] {source} message \n
        */
        const wchar* Formated() const { return _formated.c_str(); }

    private:
        String _formated;
    };
}