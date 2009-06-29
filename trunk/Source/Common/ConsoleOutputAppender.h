#pragma once

#include "LogAppender.h"

namespace P3D
{
	namespace LogAppenders
	{
		/*
		Writes log to the debug output.
		*/
		class ConsoleOutputAppender : public LogAppender
		{
		public:
			static LogAppender* CreateFromXMLDefenition(TiXmlElement* defenition);

			override void ProcessLogMessage(const LogMessage& msg);
		};
	}
}