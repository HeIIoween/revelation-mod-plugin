#pragma once

#include <string>

#include "../flhookplugin_sdk/headers/FLHook.h"

using namespace std;

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			class Print
			{
			public:
				static void EnableDebug(bool enable);

				static void Error(wstring message, wstring moduleName);
				static void Error(string message, string moduleName);

				static void Info(wstring message, wstring moduleName);
				static void Info(string message, string moduleName);

				static void Debug(wstring message, wstring moduleName);
				static void Debug(string message, string moduleName);

			protected:
				Print();
				~Print();

				static bool debugEnabled;

				static enum MessageType
				{
					M_ERROR = FOREGROUND_RED,
					M_INFO = FOREGROUND_BLUE,
					M_DEBUG = FOREGROUND_INTENSITY,
				};

				static HANDLE GetConsoleHandle();

				static void ConsolePrint(wstring message, wstring moduleName, MessageType type);
				static void ConsolePrint(string message, string moduleName, MessageType type);
			};

		}
	}
}