#include "Includes.h"
#include "FileAppender.h"
#include "UtilFunctions.h"

namespace P3D
{
    namespace LogAppenders
    {
        FileAppender::FileAppender(const String& file, bool append)
        {
            _file = _wfopen(file.c_str(), append ? L"at" : L"wt");
        }

        FileAppender::~FileAppender()
        {
            if (_file != NULL)
                fclose(_file);
        }

        void FileAppender::ProcessLogMessage(const LogMessage& msg)
        {
            if (_file != NULL)
            {
                fputws (msg.Formated(), _file);
                fflush(_file);
            }
        }

        LogAppender* FileAppender::CreateFromXMLDefenition(TiXmlElement* defenition)
        {
            String file;
            const char* fileAttr = defenition->Attribute("file");
            if (fileAttr != NULL)
                file = ToUTF16(fileAttr);
            else
                file = L"Log.txt";

            bool append = false;
            if (defenition->Attribute("append") && strcmp(defenition->Attribute("append"), "true") == 0)
                append = true;

            return new FileAppender(file, append);
        }
    }
}