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
				static DataItem::BaseData& getBase(uint id);

				static DataItem::SystemData& getSystem(wstring nickname);
				static DataItem::SystemData& getSystem(uint id);

				static DataItem::ShipData& getShip(wstring nickname);
				static DataItem::ShipData& getShip(uint id);

				static DataItem::FactionData& getFaction(wstring nickname);
				static DataItem::FactionData& getFaction(uint id);

				static DataItem::GoodsData& getGoods(wstring nickname);
				static DataItem::GoodsData& getGoods(uint id);

				static map <wstring, DataItem::BaseData> getAllBases();

			protected:
				static map <wstring, DataItem::BaseData> baseData;
				static map <wstring, DataItem::SystemData> systemData;
				static map <wstring, DataItem::ShipData> shipData;
				static map <wstring, DataItem::FactionData> factionData;
				static map <wstring, DataItem::GoodsData> goodsData;

				static map <uint, DataItem::BaseData*> baseDataIDRef;
				static map <uint, DataItem::SystemData*> systemDataIDRef;
				static map <uint, DataItem::ShipData*> shipDataIDRef;
				static map <uint, DataItem::FactionData*> factionDataIDRef;
				static map <uint, DataItem::GoodsData*> goodsDataIDRef;
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

					virtual void nickname(uint id)
					{
						eNickname = to_wstring(id);
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

				class EXPORT IDNotFound : public NickNameExecptions
				{
				public:
					virtual const char* what() const throw()
					{
						return "ID was not found";
					}
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