#include "Header.h"

#include "Data.h"
#include "DataItems.h"

#include "Client.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			EXPORT DataItem::BaseData& GetBase(wstring nickname);
			EXPORT DataItem::SystemData& GetSystem(wstring nickname);
			EXPORT DataItem::ShipData& GetShip(wstring nickname);
			EXPORT DataItem::FactionData& GetFaction(wstring nickname);
			EXPORT DataItem::GoodsData& GetGoods(wstring nickname);

			EXPORT string GetStoragePath(const char *name);

			namespace Console
			{
				EXPORT void PrintError(wstring prefix, wstring message);
				EXPORT void PrintError(string prefix, string message);

				EXPORT void PrintInfo(wstring prefix, wstring message);
				EXPORT void PrintInfo(string prefix, string message);

				EXPORT void PrintDebug(wstring prefix, wstring message);
				EXPORT void PrintDebug(string prefix, string message);
			}

			namespace Client
			{
				EXPORT Clients::Clients& Get();
			}
		}
	}
}