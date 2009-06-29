#include "Includes.h"

#include "Config.h"
#include "LoggingSystem.h"
#include "LogAppender.h"
#include "Thread.h"

#include "Exceptions.h"

#include <ctime>
#include <sys/types.h>
#include <sys/timeb.h>

#include "LogAppenderDefinitions.h"

#define LOG_BUFFER_SIZE 32768

namespace P3D
{
    DEFINE_SINGLETON(LoggingSystem);

    LoggingSystem::LoggingSystem()
    {
        if (!LoadLoggerListenerDefs())
        {
    #ifdef _DEBUG
            LogAppender* debugLogger = new LogAppenders::DebugOutputAppender();
            AddAppender(debugLogger);
            debugLogger->Release();
    #endif

            LogAppender* fileLogger = new LogAppenders::FileAppender(L"Log.txt", false);
            AddAppender(fileLogger);
            fileLogger->Release();
        }
    }

    LoggingSystem::~LoggingSystem()
    {
        Message(L"-------------------", LOG_INFO, L"System.LoggingSystem");

        std::vector<LogAppender*> localCopy;
        {
            AutoLock lock(_sync);
            // copy all references to localCopy
            for (uint i = 0; i < _listeners.size(); i++)
                localCopy.push_back(_listeners[i].Listener);
            _listeners.clear();
            // at that point we can safely release all listeners
        }
        for (uint i = 0; i < localCopy.size(); i++)
            localCopy[i]->Release();
    }

    void LoggingSystem::Message(const String& msg, LogLevel level, const String& src)
    {
        // get local time
        struct _timeb tstruct; 
        struct tm ttt;
        _ftime64_s( &tstruct );
        localtime_s(&ttt, &tstruct.time);

        // format it as string
        wchar time[100];
        swprintf_s(time, L"%02i:%02i:%02i-%03i", 
                ttt.tm_hour, ttt.tm_min, ttt.tm_sec, tstruct.millitm);

        String nameSpace;
        String object;
        uint lastDot = src.rfind('.');
        if (lastDot == -1)
        {
            nameSpace = L"";
            object = src;
        } else
        {
            nameSpace = src.substr(0, lastDot);
            object = src.substr(lastDot + 1, src.size() - lastDot - 1);
        }

        {
            AutoLock lock(_sync);
            LogMessage message(msg.c_str(), level, src.c_str(), time);

            // pass message to suitable log listener.
            for (uint i = 0; i < _listeners.size(); i++)
            {
                if (_listeners[i].Listener && ShouldLog(_listeners[i], message, nameSpace, object))
                    _listeners[i].Listener->ProcessLogMessage(message);
            }
        }
    }

    bool LoggingSystem::ShouldLog(const LoggerListenerEntry& listener, const LogMessage& message, const String& nameSpace, const String& object)
    {
        if (listener.Threshold > message.Level)
            return false;

        bool accepted = false;
        for (uint i = 0; i < listener.Accept.size(); ++i)
        {
            if (listener.Accept[i].Match(nameSpace, object))
            {
                accepted = true;
                break;
            }
        }
        if (!accepted) return false;
        
        bool rejected = false;
        for (uint i = 0; i < listener.Reject.size(); ++i)
        {
            if (listener.Reject[i].Match(nameSpace, object))
            {
                rejected = true;
                break;
            }
        }
        if (rejected) return false;

        return true;
    }

    void LoggingSystem::AddAppender(LogAppender* listener, LogLevel threshold, const String& sources)
    {
        AutoLock lock(_sync);

        listener->AddRef();
        _listeners.push_back(LoggerListenerEntry(listener, threshold, sources));
    }

    void LoggingSystem::RemoveAppender(LogAppender* listener)
    {
        AutoLock lock(_sync);
        // shitty.. but okay when _listeners is small
        while (true)
        {
            std::vector<LoggerListenerEntry>::iterator it = _listeners.begin();
            bool foundOne = false;
            while (it != _listeners.end())
            {
                if (it->Listener == listener)
                {
                    it->Listener->Release();
                    _listeners.erase(it);
                    foundOne = true;
                    break;
                } else
                    ++it;
            }
            if (!foundOne) break;
        }
    }

    bool LoggingSystem::LoadLoggerListenerDefs()
    {
        using namespace LogAppenders;

        const TiXmlElement* config = Config::GetInstance().GetSection("LoggingSystem");
        if (config == NULL) return false;
        
        TiXmlHandle doc((TiXmlNode*)config);
        TiXmlNode* node = doc.FirstChildElement("Appenders").Child(0).Node();
        while (node && node->Type() != TiXmlNode::ELEMENT)
            node = node->NextSibling();

        if (node == NULL) return false;

        TiXmlElement* loggerDef = node->ToElement();
        for( loggerDef; loggerDef; loggerDef=loggerDef->NextSiblingElement())
        {
            const char* type = loggerDef->Value();
            bool created = false;
            bool error = false;
            if (!XmlReadBool(loggerDef, "enabled", true)) continue;
            for (uint i = 0; i < sizeof(gAppendersDefenitions) / sizeof(AppenderDefenition); i++)
            {
                if (strcmp(type, gAppendersDefenitions[i].Type) != 0) continue;
                LogAppender* listener = NULL;
                try
                {
                    listener = gAppendersDefenitions[i].FactoryFunction(loggerDef);
                    if (listener == NULL)
                    { 
                        std::wostringstream os;
                        os << L"Invalid log appender defenition <" << loggerDef->Value() << L" ... /> in config.xml at line " << loggerDef->Row();
                        Message(os.str().c_str(), LOG_WARN, L"System.LoggingSystem");
                        continue;
                    }

                    const char* thresholdAttr = loggerDef->Attribute("threshold");
                    LogLevel threshold = LOG_TRACE;
                    if (thresholdAttr)
                    {
                        if (strcmp(thresholdAttr, "trace") == 0) 
                            threshold = LOG_TRACE;
                        else if (strcmp(thresholdAttr, "debug") == 0) 
                            threshold = LOG_DEBUG;
                        else if (strcmp(thresholdAttr, "info") == 0) 
                            threshold = LOG_INFO;
                        else if (strcmp(thresholdAttr, "warn") == 0) 
                            threshold = LOG_WARN;
                        else if (strcmp(thresholdAttr, "error") == 0) 
                            threshold = LOG_ERROR;
                    }

                    const char* source = loggerDef->Attribute("sources");
                    if (source == NULL)
                        AddAppender(listener);
                    else
                        AddAppender(listener, threshold, ToUTF16(source));
                    listener->Release();
                    created = true;
                    break;
                }
                catch (Exception& e)
                {
                    if (listener != NULL) RemoveAppender(listener);
                    std::wostringstream os;
                    os << L"Failed to create log appender <" << loggerDef->Value() << L" ... /> in config.xml at line " << loggerDef->Row() << L": " 
                        << e.GetFullDescription();
                    Message(os.str().c_str(), LOG_WARN, L"System.LoggingSystem");
                    error = true; 
                    break;
                }
            }
            if (!created && !error)
            {
                std::wostringstream os;
                os << L"Unrecognized log appender <" << loggerDef->Value() << L" ... /> in config.xml at line " << loggerDef->Row() << L".";
                Message(os.str().c_str(), LOG_WARN, L"System.LoggingSystem");
            }
        }

        return true;
    }

    LogMessage::LogMessage(const wchar* msg, LogLevel level, const wchar* source, const wchar* time)
        : Message(msg), Level(level), Source(source), Time(time)
    {
        std::wostringstream os;

        ThreadID = Thread::GetCurrentThreadID();

        const wchar* levelS = L"";
        switch (Level)
        {
            case LOG_TRACE: levelS = L"[TRACE] "; break;
            case LOG_DEBUG: levelS = L"[DEBUG] "; break;
            case LOG_INFO:  levelS = L"[INFO]  "; break;
            case LOG_WARN:  levelS = L"[WARN]  "; break;
            case LOG_ERROR: levelS = L"[ERROR] "; break;
        }

        os << levelS << L"<" << ThreadID << L">" 
            << L" [" << Time << L"]" 
            << L" {" << Source << L"} : " 
            << Message 
            << L"\n";

        _formated = os.str();
    }


    /////////////////////////////////////
    //// Function implementations.

    void Log(LogLevel level, const String& source, const String& message)
    {
        LoggingSystem::GetInstance().Message(message, level, source);
    }

    void Log(LogLevel level, const char* source, const char* message)
    {
        Log(level, ToUTF16(source), ToUTF16(message));
    }

}