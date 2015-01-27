#pragma once

#include <map>
#include <memory>

#include "../flhookplugin_sdk/headers/FLHook.h"

#include "SystemData.h"
#include "BaseData.h"
#include "ShipData.h"

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
					typedef map <wstring, wstring> MessageAssign;

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
					DataItem::SystemData* getSystem();

					Universe::IBase* getBaseObj();
					Archetype::Solar* getBaseArch();
					wstring getBaseNick();
					DataItem::BaseData* getBase();

					Archetype::Ship* getShipArch();
					wstring getShipNick();
					DataItem::ShipData* getShip();

					HK_ERROR sendMessage(wstring message);
					HK_ERROR sendMessage(wstring format, MessageAssign &assigns);
					HK_ERROR sendXMLMessage(wstring message);
					HK_ERROR sendXMLMessage(wstring format, MessageAssign &assigns);

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

					static void parseMessage(wstring &format, MessageAssign &assigns, wstring &result, bool isXML);
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

				namespace Exception
				{

					class EXPORT InvalidShipID : public exception
					{
					public:
						virtual const char* what() const throw()
						{
							return "Invalid ship ID";
						}
					};

					class EXPORT InvalidBaseNick : public exception
					{
					public:
						virtual const char* what() const throw()
						{
							return "Invalid base nickname";
						}
					};

					class EXPORT InvalidBaseNotFound : public exception
					{
					public:
						virtual const char* what() const throw()
						{
							return "Base can't be found";
						}
					};

				}

			}
		}
	}
}
