#pragma once

#include "LogAppender.h"

#include "DebugOutputAppender.h"
#include "FileAppender.h"
#include "ConsoleOutputAppender.h"

namespace P3D
{
	namespace LogAppenders
	{
		typedef LogAppender* (*CreateFromXMLFunction)(TiXmlElement* node);

		struct AppenderDefenition
		{
			const char* Type;
			CreateFromXMLFunction FactoryFunction;
		};

		static AppenderDefenition gAppendersDefenitions[] = 
		{
			{ "Debug", &DebugOutputAppender::CreateFromXMLDefenition },
			{ "File", &FileAppender::CreateFromXMLDefenition },
			{ "Console", &ConsoleOutputAppender::CreateFromXMLDefenition }
		};
	}
}