#pragma once

#include "Item.h"

#include "FactionData.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			namespace DataItem
			{
				class EXPORT SystemData : public Base
				{
				public:
					SystemData();
					~SystemData();

					void setFaction(wstring fNickname);
					FactionData SystemData::getFaction();

					void setTerritory(wstring fTerritory);
					wstring getTerritory();

				protected:
					wstring factionNickName = L"";
					wstring systemTerritory = L"";
				};
			}
		}
	}
}

