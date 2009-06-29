#pragma once

#include "LogAppender.h"
#include "Synchronization.h"
#include "Singleton.h"
#include "UtilFunctions.h"

namespace P3D
{
    /*
    Log singelton class. Provides login support.
    Can be extended by LogAppenders. The default appender is FileAppender to log.txt.
    Each log message has level and source information.
    */
    class LoggingSystem :
        public Singleton<LoggingSystem>
    {
        LoggingSystem();
        ~LoggingSystem();

    public:
        DECLARE_SINGLETON(LoggingSystem);

        /*
        Adds new message to the log.
        */
        void Message(const String& msg, LogLevel level, const String& source);

        /*
        Add listener to the list. Increase its reference counter.
        */
        void AddAppender(LogAppender* appender, LogLevel threshold = LOG_TRACE, const String& sources = L"*");

        /*
        Remove listener from the list and decrease its reference counter.
        */
        void RemoveAppender(LogAppender* appender);

    private:
        // guards _listeners
        Lock _sync;

        struct LoggerSource
        {
            String Namespace;
            String Object;

            LoggerSource(const wchar* src)
            {
                int size = wcslen(src);
                int i = size - 1;
                while (i > 0 && src[i] != '.')
                {
                    i--;
                }
                if (i == 0)
                {
                    Object = src;
                    Namespace = L"";
                } else
                {
                    Object = &src[i+1];
                    Namespace.assign(src, &src[i]);
                }
            }

            bool Match(const String& msgNs, const String& msgObj) const
            {
                if (wcsncmp(Namespace.c_str(), msgNs.c_str(), Namespace.length()) != 0) return false;
                if (Object == L"*") return true;
                return Object == msgObj;
            }
        };

        struct LoggerListenerEntry
        {
            LogAppender* Listener;
            LogLevel Threshold;
            
            std::vector<LoggerSource> Accept;
            std::vector<LoggerSource> Reject;

            LoggerListenerEntry(LogAppender* listener, LogLevel threshold, const String& sources)
                : Listener(listener), Threshold(threshold)
            { 
                std::vector<String> sourceParts;
                SplitString(sourceParts, sources.c_str(), ',');
                for (uint i = 0; i < sourceParts.size(); i++)
                {
                    if (sourceParts[i][0] == '!')
                        Reject.push_back(LoggerSource(&sourceParts[i][1]));
                    else
                        Accept.push_back(LoggerSource(sourceParts[i].c_str()));
                }
            }
        };

        // array of log listeners
        std::vector<LoggerListenerEntry> _listeners;

        /*
        Load logger listener defenitions from Config
        Return true in case of success
        */
        bool LoadLoggerListenerDefs();

        bool ShouldLog(const LoggerListenerEntry& listener, const LogMessage& message, const String& nameSpace, const String& object);
    };

    void Log (LogLevel level, const String& source, const String& message);
    void Log (LogLevel level, const char* source, const char* message);
}