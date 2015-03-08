#include <windows.h>

#include "Header.h"
#include "Data.h"
#include "Print.h"
#include "Hooks.h"
#include "Widget.h"
#include "Command.h"
#include "Language.h"

#include "PlayerListWidget.h"
#include "ServerStatusWidget.h"
#include "RevelationVersionCommand.h"

using namespace raincious::FLHookPlugin::Revelation;

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			PLUGIN_RETURNCODE PluginReturnCode = DEFAULT_RETURNCODE;
		}
	}
}

EXPORT PLUGIN_RETURNCODE Get_PluginReturnCode()
{
	return PluginReturnCode;
}

void hideConsole()
{
	HWND wConsole = GetConsoleWindow();

	if (wConsole == NULL)
	{
		return;
	}

	ShowWindow(wConsole, SW_HIDE);
}

void displayHelpScreen(shared_ptr<Clients::Client> client, const wstring &wscParam)
{
	Clients::Client::MessageAssign cmInfoAssign;

	Command::CommandManuals manuals = Command::Get()->manuals(client, wscParam);
	Command::CommandManuals::iterator cmIter;

	if (manuals.size() <= 0)
	{
		return;
	}

	if (wscParam == L"")
	{
		client->sendXMLMessage(
			L"<TRA color=\"aqua\" font=\"default\" bold=\"true\"/><TEXT>Revelation Mod " 
			+ Language::Get()->lang("COMMAND_HELP_COMMANDS", L"Commands")
			+ L":</TEXT>"
			);
	}
	else
	{
		cmInfoAssign[L"COUNT"] = to_wstring(manuals.size());
		cmInfoAssign[L"COMMAND"] = wscParam;

		client->sendXMLMessage(
			L"<TRA color=\"aqua\" font=\"default\" bold=\"true\"/><TEXT>"
			+ Language::Get()->lang("COMMAND_HELP_SUBCOMMANDS", L"There are ${COUNT} sub-command(s) for ${COMMAND}, type in one of it to continue:")
			+ L"</TEXT>",
			cmInfoAssign
			);
	}

	for (cmIter = manuals.begin(); cmIter != manuals.end(); ++cmIter)
	{
		Clients::Client::MessageAssign assign;
		assign[L"COMMAND"] = cmIter->Prefix;
		assign[L"PARAMETER"] = cmIter->Paramaters;
		assign[L"DESCRIPTION"] = cmIter->Description;

		if (cmIter->Paramaters != L"")
		{
			client->sendXMLMessage(
				L"<TRA color=\"aqua\" font=\"default\" bold=\"true\"/><TEXT>  ${COMMAND} ${PARAMETER}</TEXT>",
				assign
				);
		}
		else
		{
			client->sendXMLMessage(
				L"<TRA color=\"aqua\" font=\"default\" bold=\"true\"/><TEXT>  ${COMMAND}</TEXT>",
				assign
				);
		}

		if (cmIter->Description != L"")
		{
			client->sendXMLMessage(
				L"<TRA color=\"aqua\" font=\"default\" bold=\"true\"/><TEXT>   </TEXT><TRA color=\"aqua\" bold=\"default\"/><TEXT>${DESCRIPTION}</TEXT>",
				assign
				);
		}
		else
		{
			client->sendXMLMessage(
				L"<TRA color=\"aqua\" font=\"default\" bold=\"true\"/><TEXT>   </TEXT><TRA color=\"aqua\" bold=\"default\"/><TEXT>"
				+ Language::Get()->lang("COMMAND_HELP_DETAILAVAILABLE", L"Enter this command for detail.")
				+ L"</TEXT>",
				assign
				);
		}
	}
}

bool loadModInfo(string scPluginCfgFile)
{
	INI_Reader ini;
	string key;
	wstring wsKey, wsVal;

	if (!ini.open(scPluginCfgFile.c_str(), false))
	{
		return false;
	}

	try
	{
		while (ini.read_header())
		{
			if (ini.is_header("Base")) // Base Info
			{
				DataItem::BaseData base;

				while (ini.read_value())
				{
					if (ini.is_value("Nickname"))
					{
						base.setNickname(stows(ini.get_value_string()));
					}
					else if (ini.is_value("Name"))
					{
						base.setName(stows(ini.get_value_string()));
					}
					else if (ini.is_value("System"))
					{
						base.setSystem(stows(ini.get_value_string()));
					}
					else if (ini.is_value("Faction"))
					{
						base.setFaction(stows(ini.get_value_string()));
					}
				}

				Data::addBase(base);
			}
			else if (ini.is_header("System")) // System Info
			{
				DataItem::SystemData system;

				while (ini.read_value())
				{
					if (ini.is_value("Nickname"))
					{
						system.setNickname(stows(ini.get_value_string()));
					}
					else if (ini.is_value("Name"))
					{
						system.setName(stows(ini.get_value_string()));
					}
					else if (ini.is_value("Faction"))
					{
						system.setFaction(stows(ini.get_value_string()));
					}
					else if (ini.is_value("Territory"))
					{
						system.setTerritory(stows(ini.get_value_string()));
					}
				}

				Data::addSystem(system);
			}
			else if (ini.is_header("Ship")) // Ship Info
			{
				DataItem::ShipData ship;

				while (ini.read_value())
				{
					if (ini.is_value("Nickname"))
					{
						ship.setNickname(stows(ini.get_value_string()));
					}
					else if (ini.is_value("Name"))
					{
						ship.setName(stows(ini.get_value_string()));
					}
					else if (ini.is_value("Type"))
					{
						ship.setType(ini.get_value_string());
					}
				}

				Data::addShip(ship);
			}
			else if (ini.is_header("Faction")) // Faction Info
			{
				DataItem::FactionData faction;

				while (ini.read_value())
				{
					if (ini.is_value("Nickname"))
					{
						faction.setNickname(stows(ini.get_value_string()));
					}
					else if (ini.is_value("Name"))
					{
						faction.setName(stows(ini.get_value_string()));
					}
					else if (ini.is_value("Short"))
					{
						faction.setShort(stows(ini.get_value_string()));
					}
				}

				Data::addFaction(faction);
			}
			else if (ini.is_header("Goods")) // Goods Info
			{
				DataItem::GoodsData goods;

				while (ini.read_value())
				{
					if (ini.is_value("Nickname"))
					{
						goods.setNickname(stows(ini.get_value_string()));
					}
					else if (ini.is_value("Name"))
					{
						goods.setName(stows(ini.get_value_string()));
					}
				}

				Data::addGoods(goods);
			}
		}
	}
	catch (Exception::NickNameNotFound &e)
	{
		Print::Error(L"Can't load MOD data with error: "
			+ e.message()
			+ L".", L"");

		return false;
	}
	catch (Exception::RedeclaredNickname &e)
	{
		Print::Error(L"Can't load MOD data with error: "
			+ e.message()
			+ L".", L"");

		return false;
	}
	catch (exception &e)
	{
		string errorMessage = e.what();

		Print::Error(L"Can't load MOD setting with error: "
			+ wstring(errorMessage.begin(), errorMessage.end())
			+ L".", L"");

		return false;
	}

	ini.close();

	return true;
}

void __stdcall Shutdown()
{
	PluginReturnCode = DEFAULT_RETURNCODE;

	Widget::Main::Free();
}

void UserCmdHelp(uint iClientID, const wstring &wscParam)
{
	PluginReturnCode = DEFAULT_RETURNCODE;

	if (!Clients::Clients::Get().exists(iClientID))
	{
		return;
	}

	shared_ptr<Clients::Client> client = Clients::Clients::Get().get(iClientID);

	displayHelpScreen(client, wscParam);
}

bool UserCmdProcess(uint iClientID, const wstring &wscCmd)
{
	PluginReturnCode = DEFAULT_RETURNCODE;

	if (!Clients::Clients::Get().exists(iClientID))
	{
		return false;
	}

	shared_ptr<Clients::Client> client = Clients::Clients::Get().get(iClientID);
	Clients::Client::MessageAssign msgAssign;

	msgAssign[L"CMD"] = wscCmd;

	switch (Command::Get()->execute(client, wscCmd))
	{
	case Command::E_BAD:
		client->sendMessage(
			Language::Get()->lang("COMMAND_RUN_EXCEPTION", L"Error: Internal error happened when trying to execute command: ${CMD}"),
			msgAssign
			);

		PluginReturnCode = SKIPPLUGINS_NOFUNCTIONCALL;
		return true;
		break;

	case Command::E_DISABLED:
		client->sendMessage(
			Language::Get()->lang("COMMAND_RUN_DISABLED", L"Error: Command ${CMD} has been disabled."),
			msgAssign
			);

		PluginReturnCode = SKIPPLUGINS_NOFUNCTIONCALL;
		return true;
		break;

	case Command::E_OK:
		client->sendMessage(
			Language::Get()->lang("COMMAND_RUN_OK", L"OK."),
			msgAssign
			);

		PluginReturnCode = SKIPPLUGINS_NOFUNCTIONCALL;
		return true;
		break;

	case Command::E_FAILED:
		client->sendMessage(
			Language::Get()->lang("COMMAND_RUN_FAILED", L"Failed."),
			msgAssign
			);

		PluginReturnCode = SKIPPLUGINS_NOFUNCTIONCALL;
		return true;
		break;

	case Command::E_UNASSIGNED:
		displayHelpScreen(client, wscCmd);

		PluginReturnCode = SKIPPLUGINS_NOFUNCTIONCALL;
		return true;
		break;

	default: // E_NOCOMMAND etc
		break;
	}

	return false;
}

static bool pluginInited = false;
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	char szCurDir[MAX_PATH];
	
	if (!pluginInited) 
	{
		pluginInited = true;

		GetCurrentDirectory(sizeof(szCurDir), szCurDir);

		string scPluginCfgFile = string(szCurDir) + "\\flhook_plugins\\revelation.cfg";

		if (!IniGetB(scPluginCfgFile, "General", "Debug", true))
		{
			Print::EnableDebug(false);
		}

		if (IniGetB(scPluginCfgFile, "General", "HideConsole", false))
		{
			hideConsole();
		}

		Language::Get()->import(scPluginCfgFile.c_str());

		if (!loadModInfo(scPluginCfgFile))
		{
			return false;
		}

		if (IniGetB(scPluginCfgFile, "General", "EnableWidget", true))
		{
			try
			{
				Widget::Main::Width = 380;
				Widget::Main* widget = Widget::Main::Get();

				widget->mount("PlayerStatus", unique_ptr<Widget::WidgetBase>(new Widget::PlayerStatus()));
				widget->mount("ServerStatus", unique_ptr<Widget::WidgetBase>(new Widget::ServerStatus()));

				widget->run();
			}
			catch (exception &e)
			{
				Print::Error(string("Can't enable widget because: ").append(e.what()), "");
			}
		}

		try
		{
			Command::Get()->mount(
				L"/revelation",
				L"",
				Language::Get()->lang(
					"COMMAND_CMD_REVELATION_DESCRIPTION", 
					L"Display About and other information related to Revelation Mod."
					), 
				unique_ptr<CommandBase>(new Commands::RevelationVersionCommand())
				);
		}
		catch (exception &e)
		{
			Print::Error(string("Can't mount default commands because: ").append(e.what()), "");
		}
	}

	return true;
}

EXPORT PLUGIN_INFO* Get_PluginInfo()
{
	PLUGIN_INFO *p_PI = new PLUGIN_INFO();
	p_PI->sName = "Revelation Mod Data Plugin by NeXoSE <nexose@gmail.com>";
	p_PI->sShortName = "revelation";
	p_PI->bMayPause = false;
	p_PI->bMayUnload = false;
	p_PI->ePluginReturnCode = &PluginReturnCode;

	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::BaseEnter_AFTER, PLUGIN_HkIServerImpl_BaseEnter_AFTER, REVELATION_HOST_PLUGIN_CALL_LEVEL));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::PlayerLaunch_AFTER, PLUGIN_HkIServerImpl_PlayerLaunch_AFTER, REVELATION_HOST_PLUGIN_CALL_LEVEL));

	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::SystemSwitchOutComplete, PLUGIN_HkIServerImpl_SystemSwitchOutComplete, REVELATION_HOST_PLUGIN_CALL_LEVEL));

	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::Timer, PLUGIN_HkTimerCheckKick, REVELATION_HOST_PLUGIN_CALL_LEVEL));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::DisConnect, PLUGIN_HkIServerImpl_DisConnect, REVELATION_HOST_PLUGIN_CALL_LEVEL));

	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&Shutdown, PLUGIN_HkIServerImpl_Shutdown, REVELATION_HOST_PLUGIN_CALL_LEVEL));

	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmdProcess, PLUGIN_UserCmd_Process, REVELATION_HOST_PLUGIN_CALL_LEVEL));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&UserCmdHelp, PLUGIN_UserCmd_Help, REVELATION_HOST_PLUGIN_CALL_LEVEL));

	return p_PI;
}