#include "Print.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			bool Print::debugEnabled = true;

			Print::Print()
			{
			}

			Print::~Print()
			{
			}

			void Print::EnableDebug(bool enable)
			{
				debugEnabled = enable;
			}

			void Print::Error(string message, string moduleName = "")
			{
				return Error(
					wstring(message.begin(), message.end()),
					wstring(moduleName.begin(), moduleName.end())
					);
			}

			void Print::Error(wstring message, wstring moduleName = L"")
			{
				ConsolePrint(message, moduleName, M_ERROR);
			}

			void Print::Info(string message, string moduleName = "")
			{
				return Info(
					wstring(message.begin(), message.end()),
					wstring(moduleName.begin(), moduleName.end())
					);
			}

			void Print::Info(wstring message, wstring moduleName = L"")
			{
				ConsolePrint(message, moduleName, M_INFO);
			}

			void Print::Debug(string message, string moduleName = "")
			{
				return Debug(
					wstring(message.begin(), message.end()), 
					wstring(moduleName.begin(), moduleName.end())
				);
			}

			void Print::Debug(wstring message, wstring moduleName = L"")
			{
				if (debugEnabled)
				{
					ConsolePrint(message, moduleName, M_DEBUG);
				}
			}

			void Print::ConsolePrint(string message, string moduleName = "", MessageType type = M_INFO)
			{
				wstring wsMessage = wstring(message.begin(), message.end()), 
						wsModuleName = wstring(moduleName.begin(), moduleName.end());

				return ConsolePrint(wsMessage, wsModuleName, type);
			}

			void Print::ConsolePrint(wstring message, wstring moduleName = L"", MessageType type = M_INFO)
			{
				CONSOLE_SCREEN_BUFFER_INFO csbi;
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				
				if (hConsole == INVALID_HANDLE_VALUE)
				{
					return;
				}

				GetConsoleScreenBufferInfo(hConsole, &csbi);

				ConPrint(L"[Revelation]");

				if (moduleName != L"")
				{
					ConPrint(L"[" + moduleName + L"]");
				}

				ConPrint(L" ");

				SetConsoleTextAttribute(hConsole, type);

				switch (type)
				{
				case M_ERROR:
					ConPrint(L"<ERR>");
					break;

				case M_INFO:
					ConPrint(L"<INF>");
					break;

				case M_DEBUG:
					ConPrint(L"<DBG>");
					break;

				default:
					break;
				}

				SetConsoleTextAttribute(hConsole, csbi.wAttributes);

				ConPrint(L" ");

				ConPrint(message);

				ConPrint(L"\r\n");
			}
		}
	}
}