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
					typedef map <string, long> Flags;

					Client();
					~Client();

					void bind(uint& clientID);

					void renew();
					bool isValid();

					uint ID();
					bool isAdmin();

					void flag(const string key, long flagValue);
					long flag(const string key);
					Flags flag();

					wstring getName();
					wstring getAccountID();
					wstring getIP();

					bool docked(); // Not in real time

					wstring getSystemNick();
					DataItem::SystemData* getSystem();

					Universe::IBase* getBaseObj();
					uint getBaseID();
					wstring getBaseNick();
					DataItem::BaseData* getBase();

					Archetype::Ship* getShipArch();
					wstring getShipNick();
					DataItem::ShipData* getShip();

					HK_ERROR sendMessage(wstring message);
					HK_ERROR sendMessage(wstring format, MessageAssign &assigns);
					HK_ERROR sendXMLMessage(wstring message);
					HK_ERROR sendXMLMessage(wstring format, MessageAssign &assigns);

					HK_ERROR sendPrompt(wstring prefix, wstring message);
					HK_ERROR sendPrompt(wstring prefix, wstring format, MessageAssign &assigns);

					HK_ERROR kill();
					HK_ERROR beam(wstring baseNick);
					HK_ERROR ban();
					HK_ERROR kick();

				protected:
					HKPLAYERINFO playerInfo;
					PlayerData playerData;

					Flags flags;

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

					class EXPORT InvalidShipArchetypeID : public exception
					{
					public:
						virtual const char* what() const throw()
						{
							return "Invalid ship archetype ID";
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
