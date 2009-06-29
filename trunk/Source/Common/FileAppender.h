#pragma once

#include "LogAppender.h"

namespace P3D
{
    namespace LogAppenders
    {
        /*
        Writes log to the text file.
        */
        class FileAppender : public LogAppender
        {
        public:
            static LogAppender* CreateFromXMLDefenition(TiXmlElement* defenition);

            FileAppender(const String& file, bool append);
            virtual ~FileAppender();

            override void ProcessLogMessage(const LogMessage& msg);

        protected:
            FILE* _file;
        };
    }
}