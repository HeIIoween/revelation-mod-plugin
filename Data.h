#pragma once

#include <string>
#include <map>

#include "DataItems.h"

using namespace std;

namespace raincious 
{
	namespace FLHookPlugin 
	{
		namespace Revelation 
		{
			class Data
			{
			public:
				Data();
				~Data();

				static void addBase(DataItem::BaseData item);
				static void addSystem(DataItem::SystemData item);
				static void addShip(DataItem::ShipData item);
				static void addFaction(DataItem::FactionData item);
				static void addGoods(DataItem::GoodsData item);

				static DataItem::BaseData& getBase(wstring nickname);
				static DataItem::SystemData& getSystem(wstring nickname);
				static DataItem::ShipData& getShip(wstring nickname);
				static DataItem::FactionData& getFaction(wstring nickname);
				static DataItem::GoodsData& getGoods(wstring nickname);

				static map <wstring, DataItem::BaseData> getAllBases();

			protected:
				static map <wstring, DataItem::BaseData> baseData;
				static map <wstring, DataItem::SystemData> systemData;
				static map <wstring, DataItem::ShipData> shipData;
				static map <wstring, DataItem::FactionData> factionData;
				static map <wstring, DataItem::GoodsData> goodsData;
			};

			namespace Exception
			{
				class EXPORT NickNameExecptions : public exception
				{
				public:
					virtual void nickname(wstring nickname)
					{
						eNickname = nickname;
					}

					virtual wstring nickname()
					{
						return eNickname;
					}

					virtual wstring message()
					{
						string errorMessage = what();

						errorMessage.append(" (");
						errorMessage.append(wstos(eNickname));
						errorMessage.append(")");

						return wstring(errorMessage.begin(), errorMessage.end());
					}
				protected:
					wstring eNickname = L"";
				};

				class EXPORT NickNameNotFound : public NickNameExecptions
				{
				public:
					virtual const char* what() const throw()
					{
						return "Nickname was not found";
					}
				};

				class EXPORT RedeclaredNickname : public NickNameExecptions
				{
				public:
					virtual const char* what() const throw()
					{
						return "Nickname already decleared";
					}
				};
			}
		}
	}
}