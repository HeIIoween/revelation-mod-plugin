#include <windows.h>

#include "Header.h"
#include "Data.h"
#include "Print.h"
#include "Hooks.h"
#include "Widget.h"

#include "PlayerListWidget.h"
#include "ServerStatusWidget.h"

using namespace raincious::FLHookPlugin::Revelation;

PLUGIN_RETURNCODE returncode;

EXPORT PLUGIN_RETURNCODE Get_PluginReturnCode()
{
	return returncode;
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

void loadModInfo(string scPluginCfgFile)
{
	INI_Reader ini;
	string key;

	if (!ini.open(scPluginCfgFile.c_str(), false))
	{
		return;
	}

	wstring wsKey, wsVal;

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

	ini.close();
}

void __stdcall Shutdown()
{
	Widget::Main::Free();
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

		try
		{
			loadModInfo(scPluginCfgFile);
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
	p_PI->ePluginReturnCode = &returncode;

	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::BaseEnter, PLUGIN_HkIServerImpl_BaseEnter, 0));

	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::SystemSwitchOutComplete, PLUGIN_HkIServerImpl_SystemSwitchOutComplete, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::PlayerLaunch, PLUGIN_HkIServerImpl_PlayerLaunch, 0));

	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::Timer, PLUGIN_HkTimerCheckKick, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&HkIServerImpl::DisConnect, PLUGIN_HkIServerImpl_DisConnect, 0));

	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&Shutdown, PLUGIN_HkIServerImpl_Shutdown, 0));

	return p_PI;
}