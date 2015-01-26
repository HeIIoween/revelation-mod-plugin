#include "Client.h"

#include "Data.h"

#include "Print.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			namespace Clients
			{
				Clients::Clients()
				{
					Print::Debug(L"Initializing client list ...", L"");
				}

				Clients::~Clients()
				{
					ClientList::iterator iter;

					for (iter = list.begin(); iter != list.end(); iter++)
					{
						list.erase(iter->first);
					}

					Print::Debug(L"Client list has been released.", L"");
				}

				Clients& Clients::Get()
				{
					static Clients instance;

					return instance;
				}

				shared_ptr<Client> Clients::dummy()
				{
					return shared_ptr<Client>(new DummyClient());
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

					for (iter = list.begin(); iter != list.end(); iter++)
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

					for (iter = list.begin(); iter != list.end(); iter++)
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
					if (!isValid())
					{
						return;
					}

					try
					{
						lastFetchedPlayerName = Players.GetActiveCharacterName(playerClientID);

						if (lastFetchedPlayerName != playerName)
						{
							if (HKE_OK == HkGetPlayerInfo(lastFetchedPlayerName, playerInfo, false))
							{
								playerName = playerInfo.wscCharname;
								PlayerNameChar = playerInfo.wscCharname.c_str();

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
					if (!isValid())
					{
						return L"";
					}

					return playerAccount->wszAccID;
				}

				wstring Client::getIP()
				{
					if (!isValid())
					{
						return L"0.0.0.0";
					}

					return playerInfo.wscIP;
				}

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
					return &Data::getSystem(getSystemNick());
				}

				wstring Client::getBaseNick()
				{
					if (!isValid())
					{
						return L"";
					}

					return playerInfo.wscBase;
				}

				DataItem::BaseData* Client::getBase()
				{
					return &Data::getBase(getBaseNick());
				}

				HK_ERROR Client::sendMessage(wstring message)
				{
					if (!isValid())
					{
						return HKE_UNKNOWN_ERROR;
					}

					if (message.find(L"<TRA ") != string::npos)
					{
						return HkFMsg(ID(), message);
					}

					return HkMsg(ID(), message);
				}
			}
		}
	}
}