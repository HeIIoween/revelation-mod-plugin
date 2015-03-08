#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "Client.h"

using namespace std;

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{

			class EXPORT CommandBase
			{
			public:
				CommandBase();
				virtual ~CommandBase();

				virtual bool execute(shared_ptr<Clients::Client> &client, vector <wstring> &parameters);

				virtual bool isRunable(shared_ptr<Clients::Client> &client);
				virtual wstring getClientManual(shared_ptr<Clients::Client> &client);
			};

			class EXPORT Command
			{
			public:
				typedef vector <wstring> SplitedCommandPrefix;

				struct CommandManual
				{
					wstring Prefix = L"";
					wstring Paramaters = L"";
					wstring Description = L"";
				};

				struct CommandProc : public CommandManual
				{
					bool Disabled = false;
					bool Assigned = false;
					uint RunningCalls = 0;
					unique_ptr<CommandBase> Proc;
				};

				struct CommandTree : public CommandProc
				{
					map <wstring, CommandTree*> SubCommands;
				};

				typedef vector <CommandTree*> CommandTreeItems;
				typedef vector <CommandManual> CommandManuals;

				enum Error
				{
					E_OK,
					E_NOCOMMAND,
					E_FAILED,
					E_DISABLED,
					E_UNASSIGNED,
					E_BAD,
				};

				static Command* Get();

				void mount(const wstring& prefix, const wstring& parameters, const wstring& description, unique_ptr<CommandBase> commandProc);
				void demount(const wstring& prefix);

				Error execute(shared_ptr<Clients::Client> &client, const wstring &command);

				CommandManuals manuals(shared_ptr<Clients::Client> &client, const wstring &wscCmd);

			protected:
				CommandTreeItems threeItems;
				CommandTree *commandTreeRoot = nullptr;

				Command();
				~Command();

				CommandTree* locateProc(const wstring& prefix, SplitedCommandPrefix& remaining);
				CommandTree* buildProcPath(const wstring& prefix);
				void splitCommandPrefix(SplitedCommandPrefix &splited, const wstring& prefix);

				CommandTree* getTreeRoot();
			};

			namespace Exception
			{
				class EXPORT CommandPrefixNotSpecifed : public exception
				{
				public:
					virtual const char* what() const throw()
					{
						return "Command prefix must be specifed";
					}
				};

				class EXPORT CommandAlreadyAssigned : public exception
				{
				public:
					virtual const char* what() const throw()
					{
						return "Command already be assigned";
					}
				};

				class EXPORT CommandCannotUseRoot : public exception
				{
				public:
					virtual const char* what() const throw()
					{
						return "Can't change the root of command tree";
					}
				};
			}
		}
	}
}