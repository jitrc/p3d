#include "Includes.h"
#include "DebugOutputAppender.h"

namespace P3D
{
	namespace LogAppenders
	{
		void DebugOutputAppender::ProcessLogMessage(const LogMessage& msg)
		{
#if defined(P3D_WINDOWS)
			OutputDebugStringW(msg.Formated());
#endif
		}

		LogAppender* DebugOutputAppender::CreateFromXMLDefenition(TiXmlElement* defenition)
		{
			return new DebugOutputAppender();
		}
	}
}