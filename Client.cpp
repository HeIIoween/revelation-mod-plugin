#include "Client.h"
#include "Data.h"
#include "Print.h"
#include "Tools.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			namespace Clients
			{
				Exception::InvalidShipArchetypeID InvalidShipArchetypeIDException;
				Exception::InvalidBaseNick InvalidBaseNickException;
				Exception::InvalidBaseNotFound InvalidBaseNotFoundException;

				Clients::Clients()
				{
					Print::Debug(L"Initializing client list ...", L"");
				}

				Clients::~Clients()
				{
					Print::Debug(L"Releasing Client list ...", L"");
				}

				Clients& Clients::Get()
				{
					static Clients instance;

					return instance;
				}

				shared_ptr<Client> Clients::dummy()
				{
					static shared_ptr<Client> dummyInstance = shared_ptr<Client>(new DummyClient());

					return dummyInstance;
				}

				bool Clients::exists(uint& clientID)
				{
					if (list.find(clientID) == list.end())
					{
						return false;
					}

					return true;
				}

				uint Clients::size()
				{
					return list.size();
				}

				void Clients::renew(uint& clientID)
				{
					if (add(clientID))
					{
						return;
					}

					get(clientID)->renew();
				}

				void Clients::renew()
				{
					ClientList::iterator iter;

					for (iter = list.begin(); iter != list.end(); ++iter)
					{
						if (exists((uint)iter->first))
						{
							renew((uint)iter->first);
						}
					}
				}

				bool Clients::add(uint& clientID)
				{
					if (exists(clientID))
					{
						return false;
					}

					shared_ptr<Client> client = shared_ptr<Client>(new Client());

					client->bind(clientID);

					list[clientID] = client;

					Print::Debug("New client " + to_string(clientID) + " has been added.", "");

					return true;
				}

				bool Clients::remove(uint& clientID)
				{
					if (!exists(clientID))
					{
						return false;
					}

					list.erase(clientID);

					Print::Debug("Client " + to_string(clientID) + " has been removed.", "");

					return true;
				}

				shared_ptr<Client> Clients::get(uint& clientID)
				{
					if (!exists(clientID))
					{
						return dummy();
					}

					return list[clientID];
				}

				ClientList Clients::all()
				{
					return list;
				}

				ClientList Clients::inSystem(wstring systemNick)
				{
					ClientList output;

					ClientList::iterator iter;

					for (iter = list.begin(); iter != list.end(); iter++)
					{
						if (iter->second->getSystemNick() != systemNick)
						{
							continue;
						}

						output[iter->first] = iter->second;
					}

					return output;
				}

				ClientList Clients::inBase(wstring baseNick)
				{
					ClientList output;

					ClientList::iterator iter;

					for (iter = list.begin(); iter != list.end(); ++iter)
					{
						if (iter->second->getBaseNick() != baseNick)
						{
							continue;
						}

						output[iter->first] = iter->second;
					}

					return output;
				}

				DummyClient::DummyClient()
				{
					Print::Debug(L"Registering new dummy client ...", L"");

					isAssigned = false;

					playerClientID = 0;
					playerClientIDStr = L"*Dummy*";
				}

				DummyClient::~DummyClient()
				{
					Print::Debug(L"Dummy client has been released.", L"");
				}

				// Client
				Client::Client()
				{
					Print::Debug(L"Registering new client ...", L"");
				}

				void Client::bind(uint& clientID)
				{
					string sid = to_string(clientID);

					playerClientID = clientID;
					playerClientIDStr = wstring(sid.begin(), sid.end());

					if (isAssigned)
					{
						Print::Error(L"Client " + playerClientIDStr + L" already been registered.", L"");

						return;
					}

					isAssigned = true;

					renew();

					Print::Debug(L"ID " + playerClientIDStr + L" assigned to the new client.", L"");
				}

				Client::~Client()
				{
					Print::Debug(L"Client " 
						+ playerClientIDStr
						+ L" has been released."
						, L"");
				}

				uint Client::ID()
				{
					return playerClientID;
				}

				void Client::renew()
				{
					HKPLAYERINFO pi;
					CAccount *acc;

					if (!isValid())
					{
						return;
					}

					try
					{
						playerName = (const wchar_t*)Players.GetActiveCharacterName(ID());

						if (HKE_OK == HkGetPlayerInfo(playerName, pi, false))
						{
							playerInfo = pi;
							playerData = Players[ID()];

							if (lastFetchedPlayerName != playerName)
							{
								lastFetchedPlayerName = playerName;

								PlayerNameChar = playerInfo.wscCharname.c_str();

								acc = Players.FindAccountFromClientID(ID());
								accountID = acc->wszAccID;

								if ((HKE_OK == HkGetAdmin(playerName, adminRights)) && adminRights != L"")
								{
									hasAdminPermission = true;
								}
								else
								{
									hasAdminPermission = false;
								}
							}
						}
					}
					catch (exception &e)
					{
						string errMsg = e.what();

						Print::Error(
							L"Error renewing information from client " 
							+ playerClientIDStr 
							+ L": " 
							+ wstring(errMsg.begin(), errMsg.end())
							, L"");

						isAssigned = false;
					}
				}

				bool Client::isValid()
				{
					return isAssigned;
				}

				bool Client::isAdmin()
				{
					return hasAdminPermission;
				}

				Client::Flags Client::flag()
				{
					return flags;
				}

				long Client::flag(const string key)
				{
					if (flags.find(key) == flags.end())
					{
						return 0;
					}

					return flags[key];
				}

				void Client::flag(const string key, long flagValue)
				{
					if (flagValue == 0)
					{
						flags.erase(key);
					}
					else
					{
						flags[key] = flagValue;
					}
				}

				wstring Client::getName()
				{
					if (!isValid())
					{
						return L"";
					}
					
					return playerInfo.wscCharname;
				}

				wstring Client::getAccountID()
				{
					return accountID;
				}

				wstring Client::getIP()
				{
					if (!isValid())
					{
						return L"0.0.0.0";
					}

					return playerInfo.wscIP;
				}

				// System
				wstring Client::getSystemNick()
				{
					if (!isValid())
					{
						return L"";
					}

					return playerInfo.wscSystem;
				}

				DataItem::SystemData* Client::getSystem()
				{
					return Data::getSystem(getSystemNick());
				}

				// Base
				Universe::IBase* Client::getBaseObj()
				{
					uint baseID = getBaseID();

					if (baseID == 0)
					{
						throw InvalidBaseNotFoundException;
					}

					return Universe::get_base(baseID);
				}

				uint Client::getBaseID()
				{
					return Data::getBase(getBaseNick())->getID();
				}

				wstring Client::getBaseNick()
				{
					if (!isValid() || playerInfo.wscBase == L"")
					{
						throw InvalidBaseNickException;
					}

					return playerInfo.wscBase;
				}

				DataItem::BaseData* Client::getBase()
				{
					return Data::getBase(getBaseNick());
				}

				// Ship
				Archetype::Ship* Client::getShipArch()
				{
					if (!isValid() || playerData.iShipArchetype == 0)
					{
						throw InvalidShipArchetypeIDException;
					}

					return Archetype::GetShip(playerData.iShipArchetype);
				}

				wstring Client::getShipNick()
				{
					return Data::getShip(getShipArch()->iArchID)->getNickname();
				}

				DataItem::ShipData* Client::getShip()
				{
					return Data::getShip(getShipArch()->iArchID);
				}

				// Message
				void Client::parseMessage(wstring &format, MessageAssign &assigns, wstring &result, bool isXML)
				{
					wstring to = L"";
					MessageAssign::iterator iter;

					result = format;

					for (iter = assigns.begin(); iter != assigns.end(); ++iter)
					{
						to = isXML ? XMLText(iter->second) : iter->second;

						result = ReplaceStr(
							result,
							L"${" + iter->first + L"}",
							to
							);
					}
				}

				HK_ERROR Client::sendMessage(wstring message)
				{
					if (!isValid())
					{
						return HKE_UNKNOWN_ERROR;
					}

					return HkMsg(ID(), message);
				}

				HK_ERROR Client::sendMessage(wstring format, MessageAssign &assigns)
				{
					wstring parsedMessage = L"";

					if (!isValid())
					{
						return HKE_UNKNOWN_ERROR;
					}

					parseMessage(format, assigns, parsedMessage, false);

					return HkMsg(ID(), parsedMessage);
				}

				HK_ERROR Client::sendXMLMessage(wstring message)
				{
					if (!isValid())
					{
						return HKE_UNKNOWN_ERROR;
					}

					return HkFMsg(ID(), message);
				}

				HK_ERROR Client::sendXMLMessage(wstring format, MessageAssign &assigns)
				{
					wstring parsedMessage = L"";

					if (!isValid())
					{
						return HKE_UNKNOWN_ERROR;
					}

					parseMessage(format, assigns, parsedMessage, true);

					return HkFMsg(ID(), parsedMessage);
				}

				// Misc
				HK_ERROR Client::kill()
				{
					return HkKill(getName());
				}

				HK_ERROR Client::beam(wstring baseNick)
				{
					return HkBeam(getName(), baseNick);
				}

				HK_ERROR Client::ban()
				{
					return HkBan(getName(), true);
				}

				HK_ERROR Client::kick()
				{
					return HkKick(getName());
				}
			}
		}
	}
}