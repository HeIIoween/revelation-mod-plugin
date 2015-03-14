#include "Command.h"

#include "Print.h"
#include "Language.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			Exception::CommandAlreadyAssigned CommandAlreadyAssignedException;
			Exception::CommandPrefixNotSpecifed CommandPrefixNotSpecifedException;
			Exception::CommandCannotUseRoot CommandCannotUseRootException;

			CommandBase::CommandBase()
			{
			}

			CommandBase::~CommandBase()
			{
			}

			bool CommandBase::execute(shared_ptr<Clients::Client> &client, vector <wstring> &parameters)
			{
				client->sendPrompt(
					REVELATION_CMD_PREFIX,
					Language::Get()->lang("COMMAND_RUN_NOT_IMPLEMENTED", L"Error: That command is not implemented.")
					);

				return false;
			}

			void CommandBase::errored(shared_ptr<Clients::Client> &client, uint &errorCode)
			{
			}

			bool CommandBase::check(shared_ptr<Clients::Client> &client, uint &errorCode)
			{
				return true;
			}

			wstring CommandBase::manual(shared_ptr<Clients::Client> &client)
			{
				return L"";
			}

			Command* Command::Get()
			{
				static Command Instance;

				return &Instance;
			}

			Command::Command()
			{
				Print::Debug(L"Command host enabled.", L"");
			}

			Command::~Command()
			{
				CommandTreeItems::iterator cmdTreeItem;
				map <wstring, CommandTree*>::iterator treeIter;

				if (commandTreeRoot != nullptr)
				{
					for (treeIter = commandTreeRoot->SubCommands.begin(); treeIter != commandTreeRoot->SubCommands.end(); ++treeIter)
					{
						demount(treeIter->first);
					}

					delete commandTreeRoot;
				}
				
				for (cmdTreeItem = threeItems.begin(); cmdTreeItem != threeItems.end(); ++cmdTreeItem)
				{
					Print::Debug(L"Releasing command handler for: " + (*cmdTreeItem)->Prefix + L" ...", L"");

					delete (*cmdTreeItem);
				}

				Print::Debug(L"Command host released.", L"");
			}

			Command::CommandManuals Command::manuals(shared_ptr<Clients::Client> &client, const wstring &wscCmd)
			{
				return manuals(client, wscCmd, false);
			}

			Command::CommandManuals Command::manuals(shared_ptr<Clients::Client> &client, const wstring &wscCmd, bool onlyUseable)
			{
				uint subCmds = 0, tempError = 0;
				CommandManuals manualList;
				map <wstring, CommandTree*>::iterator cmdTreeIter;
				SplitedCommandPrefix parameter;
				CommandTree *proc = locateProc(wscCmd, parameter);
				wstring tmpDescription = L"";

				for (cmdTreeIter = proc->SubCommands.begin(); cmdTreeIter != proc->SubCommands.end(); ++cmdTreeIter)
				{
					CommandManual man;

					man.Useable = true;

					if (cmdTreeIter->second->Disabled)
					{
						continue;
					}

					if (!cmdTreeIter->second->Proc->check(client, tempError))
					{
						if (onlyUseable)
						{
							continue;
						}

						man.Useable = false;
					}
					
					if (cmdTreeIter->second->SubCommands.size() > 0)
					{
						// Load commands only when we able to use it. 
						if (manuals(client, cmdTreeIter->second->Prefix, true).size() < 1)
						{
							if (onlyUseable)
							{
								continue;
							}

							man.Useable = false;
						}
					}

					tmpDescription = cmdTreeIter->second->Proc->manual(client);
					if (tmpDescription == L"")
					{
						tmpDescription = cmdTreeIter->second->Description;
					}

					man.Description = tmpDescription;
					man.Prefix = cmdTreeIter->second->Prefix;
					man.Paramaters = cmdTreeIter->second->Paramaters;

					manualList.push_back(man);
				}

				return manualList;
			}

			Command::Error Command::execute(shared_ptr<Clients::Client> &client, const wstring &command)
			{
				Error result = E_OK;
				uint errorCode = 0;
				SplitedCommandPrefix parameter;
				SplitedCommandPrefix::iterator pIter;
				vector <wstring> parameterData;
				CommandProc *proc = locateProc(command, parameter);
				
				if (proc == getTreeRoot()) // If it ain't go anywhere.
				{
					return E_NOCOMMAND;
				}

				if (!proc->Assigned)
				{
					return E_UNASSIGNED;
				}

				if (proc->Disabled)
				{
					return E_DISABLED;
				}

				if (!proc->Proc->check(client, errorCode))
				{
					proc->Proc->errored(client, errorCode);

					return E_FAILED_CHECK;
				}

				for (pIter = parameter.begin(); pIter != parameter.end(); ++pIter)
				{
					parameterData.push_back(*pIter);
				}

				proc->RunningCalls++;
				try 
				{
					if (!proc->Proc->execute(client, parameterData))
					{
						result = E_FAILED;
					}
				}
				catch (exception &e)
				{
					Print::Error(string("An error happened when trying to execute user command: ").append(e.what()), "");

					result = E_BAD;
				}
				proc->RunningCalls--;

				return result;
			}

			void Command::mount(const wstring& prefix, const wstring& parameters, const wstring& description, unique_ptr<CommandBase> commandProc)
			{
				CommandTree *proc = nullptr;

				Print::Debug(L"Mounting command " + prefix + L" ...", L"");

				if (prefix == L"")
				{
					throw CommandPrefixNotSpecifedException;
				}

				proc = buildProcPath(prefix);

				if (proc == getTreeRoot())
				{
					throw CommandCannotUseRootException;
				}

				if (proc->Assigned) // If it's already be assigned.
				{
					throw CommandAlreadyAssignedException;
				}

				proc->Prefix = prefix;
				proc->Paramaters = parameters;
				proc->Description = description;

				proc->Proc = move(commandProc);
				proc->Disabled = false;
				proc->Assigned = true;
			}

			void Command::demount(const wstring& prefix)
			{
				map <wstring, CommandTree*>::iterator subIter;
				SplitedCommandPrefix remaining;
				CommandTree *proc = locateProc(prefix, remaining);

				if (proc == getTreeRoot())
				{
					return;
				}

				Print::Debug(L"Disabling command " + proc->Prefix + L" ...", L"");

				for (subIter = proc->SubCommands.begin(); subIter != proc->SubCommands.end(); ++subIter)
				{
					demount(subIter->second->Prefix);
				}

				while (proc->RunningCalls > 0)
				{
					Sleep(100);
				}

				proc->Disabled = true;
				proc->Assigned = false;
				proc->Proc = unique_ptr<CommandBase>(new CommandBase());

				Print::Debug(L"Command " + proc->Prefix + L" has been disabled.", L"");
			}

			Command::CommandTree* Command::getTreeRoot()
			{
				if (commandTreeRoot == nullptr)
				{
					commandTreeRoot = new CommandTree();

					commandTreeRoot->Prefix = L"";
					commandTreeRoot->Paramaters = L"";
					commandTreeRoot->Description = L"";
					commandTreeRoot->Disabled = true;
					commandTreeRoot->Assigned = false;
					commandTreeRoot->Proc = unique_ptr<CommandBase>(new CommandBase());
				}

				return commandTreeRoot;
			}

			Command::CommandTree* Command::locateProc(const wstring& prefix, SplitedCommandPrefix& remaining)
			{
				CommandTree *buildedProcPath = getTreeRoot(); // Search from root

				SplitedCommandPrefix::iterator splitedIter;

				splitCommandPrefix(remaining, prefix);

				for (splitedIter = remaining.begin(); splitedIter != remaining.end();)
				{
					// Different behavior: We don't create anything, just enter enter enter
					if (buildedProcPath->SubCommands.find((*splitedIter)) == buildedProcPath->SubCommands.end())
					{
						break;
					}

					// Use the new tree branch for next round
					buildedProcPath = buildedProcPath->SubCommands[(*splitedIter)];

					splitedIter = remaining.erase(splitedIter); // Remove used key
				}

				return buildedProcPath; // Return the deepest branch
			}

			Command::CommandTree* Command::buildProcPath(const wstring& prefix)
			{
				CommandTree *buildedProcPath = getTreeRoot(); // Goto root by default
				SplitedCommandPrefix splited;
				SplitedCommandPrefix::iterator splitedIter;

				splitCommandPrefix(splited, prefix);

				for (splitedIter = splited.begin(); splitedIter != splited.end(); ++splitedIter)
				{
					// Check if this tree branch is existed, if not, create it.
					if (buildedProcPath->SubCommands.find((*splitedIter)) == buildedProcPath->SubCommands.end())
					{
						// Create a new item, chaos in heap
						buildedProcPath->SubCommands[(*splitedIter)] = new CommandTree();

						buildedProcPath->SubCommands[(*splitedIter)]->Prefix = 
							buildedProcPath->Prefix != L"" ? (buildedProcPath->Prefix + L" " + (*splitedIter)) : (*splitedIter);

						buildedProcPath->SubCommands[(*splitedIter)]->Proc = unique_ptr<CommandBase>(new CommandBase()); // Default Command Handler
						buildedProcPath->SubCommands[(*splitedIter)]->Disabled = false;
						buildedProcPath->SubCommands[(*splitedIter)]->Assigned = false;

						// Backup the object address for release.
						threeItems.push_back(buildedProcPath->SubCommands[(*splitedIter)]);
					}

					// Use the new tree branch for next round
					buildedProcPath = buildedProcPath->SubCommands[(*splitedIter)];
				}

				return buildedProcPath; // Return the deepest branch
			}

			void Command::splitCommandPrefix(SplitedCommandPrefix &splited, const wstring& prefix)
			{
				wstring cut = L"";
				wstring::size_type lastFoundPos = 0, currentFoundPos = 0, subSearchPos = 0, nextPos = 0;
				wchar_t lookingFor = L'';

				while ((currentFoundPos = prefix.find(L' ', lastFoundPos)) != wstring::npos)
				{
					nextPos = currentFoundPos + 1;

					if (lookingFor == L'')
					{
						if (prefix[nextPos] == L'"')
						{
							lookingFor = L'"';

							nextPos++;
						}
						else if (prefix[nextPos] == L'\'')
						{
							lookingFor = L'\'';

							nextPos++;
						}
					}
					else
					{
						// Go through all characters to find ending ' or "
						while ((subSearchPos = prefix.find(lookingFor, nextPos)) != wstring::npos)
						{
							nextPos = subSearchPos + 1;

							if (prefix[subSearchPos - 1] == L'\\')
							{
								continue;
							}

							currentFoundPos = subSearchPos;

							break;
						}

						// Dismiss ending search
						lookingFor = L'';
					}

					cut = prefix.substr(lastFoundPos, currentFoundPos - lastFoundPos);

					lastFoundPos = nextPos;

					if (cut == L"")
					{
						continue;
					}

					splited.push_back(cut);
				}

				cut = prefix.substr(lastFoundPos, currentFoundPos - lastFoundPos);

				if (cut != L"")
				{
					splited.push_back(cut);
				}
			}
		}
	}
}