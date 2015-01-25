#include "PlayerListWidget.h"

#include "Print.h"
#include "Data.h"
#include "Tools.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			namespace Widget
			{

				PlayerStatus::~PlayerStatus()
				{
					MenuGroup::iterator mIter;

					RemoveWidgetSubClass(this);

					for (mIter = systemMenus.begin(); mIter != systemMenus.end(); ++mIter)
					{
						DestroyMenu(*mIter);
					}

					for (mIter = territoryMenus.begin(); mIter != territoryMenus.end(); ++mIter)
					{
						DestroyMenu(*mIter);
					}

					if (shortcutMenuLandTo != NULL)
					{
						DestroyMenu(shortcutMenuLandTo);
					}

					if (shortcutMenu != NULL)
					{
						DestroyMenu(shortcutMenu);
					}
				}

				void PlayerStatus::create(HWND parent, uint top, uint left, uint width, uint height)
				{
					DWORD beamDIndex = ID_LOCATIONBEAM_CTXMENU_START;

					InitCommonControls();

					widget = CreateWindowEx(0L,
						WC_LISTVIEW,
						TEXT("Revelation Player Status"),
						LVS_REPORT | LVS_SINGLESEL | LVS_EX_GRIDLINES | WS_CHILD,
						left + 10,
						top + 10,
						width - 20,
						height - 100,
						parent,
						NULL,
						(HINSTANCE)GetModuleHandle(NULL),
						NULL);

					if (widget == NULL)
					{
						return;
					}

					ListView_SetExtendedListViewStyle(widget, LVS_EX_FULLROWSELECT);

					LVCOLUMN lvcID, lvcName, lvcSystem, lvcIP, lvcFlags;

					lvcID.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
					lvcID.iSubItem = 0;
					lvcID.pszText = TEXT("ID");
					lvcID.cx = 25;
					lvcID.fmt = LVCFMT_LEFT;

					lvcName.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
					lvcName.iSubItem = 1;
					lvcName.pszText = TEXT("Player");
					lvcName.cx = 100;
					lvcName.fmt = LVCFMT_LEFT;

					lvcSystem.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
					lvcSystem.iSubItem = 2;
					lvcSystem.pszText = TEXT("System");
					lvcSystem.cx = 80;
					lvcSystem.fmt = LVCFMT_LEFT;

					lvcIP.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
					lvcIP.iSubItem = 3;
					lvcIP.pszText = TEXT("IP Address");
					lvcIP.cx = 80;
					lvcIP.fmt = LVCFMT_LEFT;

					lvcFlags.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
					lvcFlags.iSubItem = 4;
					lvcFlags.pszText = TEXT("Flags");
					lvcFlags.cx = 50;
					lvcFlags.fmt = LVCFMT_LEFT;

					ListView_InsertColumn(widget, 0, &lvcID);
					ListView_InsertColumn(widget, 1, &lvcName);
					ListView_InsertColumn(widget, 2, &lvcSystem);
					ListView_InsertColumn(widget, 3, &lvcIP);
					ListView_InsertColumn(widget, 4, &lvcFlags);

					// Init base menu data
					map <wstring, DataItem::BaseData>::iterator baseIter;
					map <wstring, DataItem::BaseData> bases = Data::getAllBases();

					for (baseIter = bases.begin(); baseIter != bases.end(); ++baseIter)
					{
						LandBase & bD = territoryList[ws2s(baseIter->second.getSystem()->getTerritory())][ws2s(baseIter->second.getSystem()->getName())][beamDIndex];

						bD.Name = ws2s(baseIter->second.getName());
						bD.Nickname = ws2s(baseIter->second.getNickname());
						bD.SystemNick = ws2s(baseIter->second.getSystem()->getNickname());

						beamDestinations[beamDIndex] = bD;

						++beamDIndex;
					}

					ID_LOCATIONBEAM_CTXMENU_END = beamDIndex;

					buildMenu();

					ShowWindow(widget, SW_SHOW);

					SetWidgetSubClassProc(this);
				}

				void PlayerStatus::buildMenu()
				{
					// Creating shortcut menu
					shortcutMenu = CreatePopupMenu();
					shortcutMenuLandTo = CreateMenu();

					LandTerritoryList::iterator landTerritoryIter;
					LandSystemList::iterator landSystemIter;
					LandBaseList::iterator landBaseIter;
					for (landTerritoryIter = territoryList.begin(); landTerritoryIter != territoryList.end(); ++landTerritoryIter)
					{
						HMENU systemMenu = CreateMenu();
						uint systemMenuID = 0;

						for (landSystemIter = territoryList[landTerritoryIter->first].begin(); landSystemIter != territoryList[landTerritoryIter->first].end(); ++landSystemIter)
						{

							HMENU baseMenu = CreateMenu();
							uint baseMenuID = 0;

							// Add bases to the menu item one by one
							for (landBaseIter = territoryList[landTerritoryIter->first][landSystemIter->first].begin(); landBaseIter != territoryList[landTerritoryIter->first][landSystemIter->first].end(); ++landBaseIter)
							{
								InsertMenu(baseMenu, baseMenuID, MF_BYCOMMAND | MF_STRING | MF_ENABLED, landBaseIter->first, landBaseIter->second.Name.c_str());
							}

							systemMenus.push_back(baseMenu);

							InsertMenu(systemMenu, 0, MF_STRING | MF_POPUP, (UINT)baseMenu, landSystemIter->first.c_str());
						}

						territoryMenus.push_back(systemMenu);

						InsertMenu(shortcutMenuLandTo, 0, MF_STRING | MF_POPUP, (UINT)systemMenu, landTerritoryIter->first.c_str());
					}

					// Beam menu
					InsertMenu(shortcutMenu, 0, MF_STRING | MF_POPUP, (UINT)shortcutMenuLandTo, TEXT("&Move to"));
					InsertMenu(shortcutMenu, 1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, ID_PLAYERSTATUS_CTXMENU_FRIENDLYCURRENTBASE, TEXT("&Friendly with current base"));
					InsertMenu(shortcutMenu, 2, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
					InsertMenu(shortcutMenu, 3, MF_BYCOMMAND | MF_STRING | MF_ENABLED, ID_PLAYERSTATUS_CTXMENU_CLEARCASH, TEXT("&Remove all credits"));
					InsertMenu(shortcutMenu, 4, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
					InsertMenu(shortcutMenu, 5, MF_BYCOMMAND | MF_STRING | MF_ENABLED, ID_PLAYERSTATUS_CTXMENU_KICK, TEXT("&Kick"));
					InsertMenu(shortcutMenu, 6, MF_BYCOMMAND | MF_STRING | MF_ENABLED, ID_PLAYERSTATUS_CTXMENU_BAN, TEXT("&Ban"));
					InsertMenu(shortcutMenu, 7, MF_BYCOMMAND | MF_STRING | MF_ENABLED, ID_PLAYERSTATUS_CTXMENU_KILL, TEXT("&Kill"));
				}

				void PlayerStatus::onPaint(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, HDC windowClientArea)
				{
					UpdateWindow(widget);
				}

				void PlayerStatus::onSize(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, uint top, uint left, uint width, uint height)
				{
					SetWindowPos(
						widget,
						NULL,
						left + 10,
						top + 10,
						width - 20,
						height - 100,
						SWP_NOZORDER | SWP_NOACTIVATE
						);

					UpdateWindow(widget);
				}

				void PlayerStatus::tick()
				{
					Clients::ClientList::iterator clientIter;
					Clients::ClientList clients = Clients::Clients::Get().all();

					Players empty;

					Players::iterator pIter;
					for (pIter = newPlayers.begin(); newPlayers.end() != pIter; ++pIter)
					{
						delete *pIter;
					}

					newPlayers.swap(empty);

					for (clientIter = clients.begin(); clients.end() != clientIter; ++clientIter)
					{
						PlayerInfo* player = new PlayerInfo();

						player->ID = to_string(clientIter->second->ID());
						player->Name = ws2s(clientIter->second->getName());

						try
						{
							player->System = ws2s(clientIter->second->getSystem().getName());
						}
						catch (Exception::NickNameNotFound &e)
						{
							e.what();

							player->System = ws2s(clientIter->second->getSystemNick());
						}

						player->IP = ws2s(clientIter->second->getIP());

						if (clientIter->second->isAdmin())
						{
							player->Flags.append("Admin ");
						}

						player->cID = player->ID.c_str();
						player->cName = player->Name.c_str();
						player->cSystem = player->System.c_str();
						player->cIP = player->IP.c_str();
						player->cFlags = player->Flags.c_str();

						newPlayers.push_back(player);
					}

					players.swap(newPlayers);

					update();
				}

				void PlayerStatus::update()
				{
					LVITEM lvI;
					uint ulIndex = 0;
					Players::iterator pIter;
					int autoSelectID = ListView_GetNextItem(widget, -1, LVNI_SELECTED);

					ListView_DeleteAllItems(widget);

					lvI.mask = LVIF_TEXT;

					for (pIter = players.begin(); players.end() != pIter; ++pIter)
					{
						lvI.iItem = ulIndex++;

						lvI.iSubItem = 0;
						lvI.pszText = (LPSTR)(*pIter)->cID;
						ListView_InsertItem(widget, &lvI);

						lvI.iSubItem = 1;
						lvI.pszText = (LPSTR)(*pIter)->cName;
						ListView_SetItem(widget, &lvI);

						lvI.iSubItem = 2;
						lvI.pszText = (LPSTR)(*pIter)->cSystem;
						ListView_SetItem(widget, &lvI);

						lvI.iSubItem = 3;
						lvI.pszText = (LPSTR)(*pIter)->cIP;
						ListView_SetItem(widget, &lvI);

						lvI.iSubItem = 4;
						lvI.pszText = (LPSTR)(*pIter)->cFlags;
						ListView_SetItem(widget, &lvI);
					}

					if (autoSelectID != -1 && lvI.iItem >= autoSelectID)
					{
						ListView_SetItemState(widget, autoSelectID, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);
					}
				}

				LRESULT CALLBACK PlayerStatus::proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
				{
					POINT ppt;
					int selectID = -1;
					char menuItemTitle[256] = "";
					char *menuItemTitleBuf = menuItemTitle;
					HK_ERROR commandError;

					switch (uMsg)
					{
					case WM_CONTEXTMENU:
						selectID = ListView_GetNextItem(widget, -1, LVNI_SELECTED);

						if (selectID != -1)
						{
							ListView_GetItemPosition(widget, selectID, &ppt);
							ClientToScreen(widget, &ppt);

							SetForegroundWindow(widget);
							TrackPopupMenu(shortcutMenu, TPM_TOPALIGN | TPM_LEFTALIGN, ppt.x, ppt.y, 0, widget, NULL);
							PostMessage(widget, WM_NULL, 0, 0);
						}

						return 0;
						break;

					case WM_COMMAND:
						selectID = ListView_GetNextItem(widget, -1, LVNI_SELECTED);

						if (selectID != -1)
						{
							ListView_GetItemText(widget, selectID, 0, menuItemTitleBuf, 256);

							try
							{
								commandError = commands(wParam, stoi(menuItemTitleBuf));

								if (commandError != HKE_OK)
								{
									MessageBox(
										widget,
										string("An error has occurred when sending command to selected client: ")
										.append(menuItemTitleBuf)
										.append(".\r\n\r\nError: ").append(ws2s(HkErrGetText(commandError)))
										.append(".\r\n\r\nPlease verify and try again.")
										.c_str(),
										"Error executing command",
										MB_ICONWARNING
										);
								}
							}
							catch (exception &e)
							{
								MessageBox(
									widget,
									string("An application exception has occurred when sending command to selected client: ")
									.append(menuItemTitleBuf)
									.append(".\r\n\r\nError: ").append(e.what())
									.append(".\r\n\r\nPlease verify and try again.")
									.c_str(),
									"Error executing command",
									MB_ICONWARNING
									);
							}
						}

						break;
					}
					
					return CallWidgetDefaultProc(uIdSubclass, widget, uMsg, wParam, lParam);
				}

				HK_ERROR PlayerStatus::commands(uint commandID, uint clientID)
				{
					wstring playerName = L"";
					Clients::Clients &client = Clients::Clients::Get();

					if (clientID <= 0)
					{
						return HKE_INVALID_CLIENT_ID;
					}

					switch (commandID)
					{
					case ID_PLAYERSTATUS_CTXMENU_FRIENDLYCURRENTBASE:
						return commandFriendlyBase(client.get(clientID));
						break;

					case ID_PLAYERSTATUS_CTXMENU_CLEARCASH:
						return commandCash(commandID, client.get(clientID));
						break;

					case ID_PLAYERSTATUS_CTXMENU_KILL:
						if (HkKill(client.get(clientID)->getName()) == HKE_OK)
						{
							client.get(clientID)->sendMessage(L"You've killed by server admin.");

							return HKE_OK;
						}
						break;

					case ID_PLAYERSTATUS_CTXMENU_BAN:
						playerName = client.get(clientID)->getName();

						client.get(clientID)->sendMessage(L"You have been banned by server admin.");

						if (HkBan(playerName, true) == HKE_OK)
						{
							return HkKick(playerName);
						}
						break;

					case ID_PLAYERSTATUS_CTXMENU_KICK:
						if (HkKickReason(
							client.get(clientID)->getName(), 
							L"You are kicked from server by server admin.") == HKE_OK)
						{
							return HKE_OK;
						}
						break;

					default:
						if (commandID > ID_LOCATIONBEAM_CTXMENU_START && commandID < ID_LOCATIONBEAM_CTXMENU_END)
						{
							return commandBeam(commandID, client.get(clientID));
						}

						break;
					}

					return HKE_UNKNOWN_ERROR;
				}

				HK_ERROR PlayerStatus::commandCash(uint commandID, Clients::Client *client)
				{
					int iCash;
					HK_ERROR error;

					error = HkGetCash(client->getName(), iCash);

					if (error != HKE_OK) {
						return error;
					}

					client->sendMessage(L"All of your credits has been removed by server admin.");

					return HkAddCash(client->getName(), 0 - iCash);
				}

				HK_ERROR PlayerStatus::commandBeam(uint commandID, Clients::Client *client)
				{
					wstring playerName = L"", wscCharFileName = L"";
					HK_ERROR error = HKE_UNKNOWN_ERROR;
					HKPLAYERINFO playerInfo;
					uint baseID = 0, playerOldSystemID = 0;
					float baseHealth = 0.0, baseMaxHealth = 0.0;
					CHARACTER_ID cID;

					if (beamDestinations.find(commandID) == beamDestinations.end())
					{
						return HKE_INVALID_BASENAME;
					}

					try 
					{
						if (HkGetBaseStatus(stows(beamDestinations[commandID].Nickname), baseHealth, baseMaxHealth))
						{
							return HKE_INVALID_BASENAME;
						}

						if (!HKHKSUCCESS(HkGetPlayerInfo(client->getName(), playerInfo, false)))
						{
							return HKE_NO_MATCHING_PLAYER;
						}

						if (playerInfo.iShip != 0)
						{
							client->sendMessage(
								L"Server admin wants to move you to "
								+ stows(beamDestinations[commandID].Name)
								+ L". But that could only be possible when you not in space."
								);

							MessageBox(widget, "Player must land on a base in order to be moved.", "Moving player", MB_ICONWARNING);

							return HKE_OK;
						}

						client->sendMessage(
							L"Server admin is moving your to "
							+ stows(beamDestinations[commandID].Name)
							+ L". Please be prepared."
							);

						// Make sure they never come back
						playerName = client->getName();

						error = HkBan(playerName, true);
						if (error != HKE_OK)
						{
							return error;
						}

						error = HkKick(playerName);
						if (error != HKE_OK)
						{
							return error;
						}

						error = HkGetCharFileName(playerName, wscCharFileName);
						if (error != HKE_OK)
						{
							return error;
						}

						IniEditItemSet iniSet;
						IniEditItems iniItem;

						iniItem.Node = "Player";
						iniItem.Key = "system";
						iniItem.NewValue = beamDestinations[commandID].SystemNick;
						iniSet.push_back(iniItem);

						iniItem.Node = "Player";
						iniItem.Key = "base";
						iniItem.NewValue = beamDestinations[commandID].Nickname;
						iniSet.push_back(iniItem);

						iniItem.Node = "Player";
						iniItem.Key = "last_base";
						iniItem.NewValue = beamDestinations[commandID].Nickname;
						iniSet.push_back(iniItem);

						// Edit file
						error = editPlayerCharFile(playerName, iniSet);
						if (error != HKE_OK)
						{
							return error;
						}

						// Unban player
						error = HkBan(playerName, false);
						if (error != HKE_OK)
						{
							return error;
						}

						return HKE_OK;
					}
					catch (...) 
					{
						// Do thing here
					}

					return error;
				}

				HK_ERROR PlayerStatus::commandFriendlyBase(Clients::Client *client)
				{
					HK_ERROR err = HKE_UNKNOWN_ERROR;
					HKPLAYERINFO playerInfo;

					if (!HKHKSUCCESS(HkGetPlayerInfo(client->getName(), playerInfo, false)))
					{
						return HKE_NO_MATCHING_PLAYER;
					}

					if (playerInfo.iShip != 0)
					{
						MessageBox(widget, "Player must land on the base.", "Base friendly", MB_ICONWARNING);

						return HKE_OK;
					}

					err = HkSetRep(
						client->getName(), 
						Data::getBase(playerInfo.wscBase).getFaction()->getNickname(),
						(float)0.65
						);
					if (err == HKE_OK)
					{
						client->sendMessage(
							L"You are now friendly with the faction which runing the base you currently docked."
							);

						return HKE_OK;
					}

					return err;
				}
			}
		}
	}
}