#pragma once

#include <map>
#include <memory>

#include "../flhookplugin_sdk/headers/FLHook.h"

#include "SystemData.h"
#include "BaseData.h"

using namespace std;

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			namespace Clients
			{

				class EXPORT Client
				{
				public:
					Client();
					~Client();

					void bind(uint& clientID);

					void renew();
					bool isValid();

					uint ID();
					bool isAdmin();

					wstring getName();
					wstring getAccountID();
					wstring getIP();

					wstring getSystemNick();
					DataItem::SystemData getSystem();

					wstring getBaseNick();
					DataItem::BaseData getBase();

					HK_ERROR sendMessage(wstring message);

				protected:
					HKPLAYERINFO playerInfo;
					CAccount *playerAccount;

					wstring playerName = L"", lastFetchedPlayerName = L"";
					const wchar_t* PlayerNameChar = L"";

					wstring accountID = L"";

					wstring playerClientIDStr = L"0";

					uint playerClientID = 0;

					wstring adminRights = L"";

					bool isAssigned = false;
					bool hasAdminPermission = false;
				};

				class DummyClient : public Client
				{
				public:
					DummyClient();
					~DummyClient();
				};

				typedef EXPORT map <uint, shared_ptr<Client>> ClientList;

				class EXPORT Clients
				{
				public:
					static Clients& Get();
					static shared_ptr<Client> dummy();

					bool add(uint& clientID);
					bool remove(uint& clientID);
					shared_ptr<Client> get(uint& clientID);
					bool exists(uint& clientID);

					void renew(uint& clientID);
					void renew();

					uint size();

					ClientList inSystem(wstring systemNick);
					ClientList inBase(wstring baseNick);
					ClientList all();

				protected:
					ClientList list;
					
					Clients();
					~Clients();
				};
			}

		}
	}
}
