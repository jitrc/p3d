#include "Includes.h"
#include "ConsoleOutputAppender.h"
#include "UtilFunctions.h"

namespace P3D
{
    namespace LogAppenders
    {
        void ConsoleOutputAppender::ProcessLogMessage(const LogMessage& msg)
        {
            std::wcout << msg.Formated();
        }

        LogAppender* ConsoleOutputAppender::CreateFromXMLDefenition(TiXmlElement* defenition)
        {
            OpenConsole(XmlReadBool(defenition, "attachToParent", true));
            return new ConsoleOutputAppender();
        }
    }
}