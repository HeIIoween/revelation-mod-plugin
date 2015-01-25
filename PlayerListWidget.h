#pragma once

#pragma warning(disable: 4503)

#include <windows.h>
#include <commctrl.h>
#include <memory>

#include "Widget.h"
#include "Data.h"
#include "Client.h"

#include "../flhookplugin_sdk/headers/FLHook.h"

#define ID_PLAYERSTATUS_CTXMENU_LOCATIONBEAM 300010
#define ID_PLAYERSTATUS_CTXMENU_FRIENDLYCURRENTBASE 300011
#define ID_PLAYERSTATUS_CTXMENU_CLEARCASH 300012
#define ID_PLAYERSTATUS_CTXMENU_KICK 300013
#define ID_PLAYERSTATUS_CTXMENU_BAN 300014
#define ID_PLAYERSTATUS_CTXMENU_KILL 300015

#define ID_LOCATIONBEAM_CTXMENU_START 300016 // Make sure this be the last and greatest number

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			namespace Widget
			{

				typedef struct PlayerInfo
				{
					string ID;
					string Name;
					string System;
					string IP;
					string Flags;

					const char *cID;
					const char *cName;
					const char *cSystem;
					const char *cIP;
					const char *cFlags;
				} playerInfo;

				typedef struct LandBase
				{
					string Nickname = "";
					string Name = "";
					string SystemNick = "";
				} landBase;

				typedef map <DWORD, LandBase> LandBaseList;
				typedef map <string, LandBaseList> LandSystemList;
				typedef map <string, LandSystemList> LandTerritoryList;

				typedef vector <PlayerInfo*> Players;

				typedef vector <HMENU> MenuGroup;

				class PlayerStatus : public WidgetBase
				{
				public:
					virtual ~PlayerStatus();

					void create(HWND parent, uint top, uint left, uint width, uint height);
					void onSize(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, uint top, uint left, uint width, uint height);
					
					void onPaint(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, HDC windowClientArea);
					void tick();

				protected:
					DWORD ID_LOCATIONBEAM_CTXMENU_END = 0;
					HMENU shortcutMenu = NULL, shortcutMenuLandTo = NULL;
					COLORREF defaultRowTextColor;
					Players players, newPlayers;
					LandBaseList beamDestinations;
					MenuGroup territoryMenus, systemMenus;
					LandTerritoryList territoryList;

					void buildMenu();
					LRESULT CALLBACK proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
					void update();

					HK_ERROR commands(uint commandID, uint clientID);
					HK_ERROR commandBeam(uint commandID, shared_ptr<Clients::Client> client);
					HK_ERROR commandCash(uint commandID, shared_ptr<Clients::Client> client);
					HK_ERROR commandFriendlyBase(shared_ptr<Clients::Client> client);
				};

			}
		}
	}
}