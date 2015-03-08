#include "Header.h"

#include "Data.h"
#include "DataItems.h"

#include "Client.h"
#include "Command.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			EXPORT DataItem::BaseData* GetBase(uint uid);
			EXPORT DataItem::BaseData* GetBase(wstring nickname);

			EXPORT DataItem::SystemData* GetSystem(uint uid);
			EXPORT DataItem::SystemData* GetSystem(wstring nickname);

			EXPORT DataItem::ShipData* GetShip(uint uid);
			EXPORT DataItem::ShipData* GetShip(wstring nickname);

			EXPORT DataItem::FactionData* GetFaction(uint uid);
			EXPORT DataItem::FactionData* GetFaction(wstring nickname);

			EXPORT DataItem::GoodsData* GetGoods(uint uid);
			EXPORT DataItem::GoodsData* GetGoods(wstring nickname);

			EXPORT string GetStoragePath(const char *name);
			EXPORT int GetRandNumber(int min, int max);

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

			namespace Cmd
			{
				EXPORT void Mount(const wstring& prefix, const wstring& paramater, const wstring& description, unique_ptr<CommandBase> commandProc);
				EXPORT void Demount(const wstring& prefix);
			}

			namespace Lang
			{
				EXPORT const wstring& Get(const string key, const wstring defaultStr);
				EXPORT void Import(const char* path);
			}
		}
	}
}