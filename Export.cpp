#include "Export.h"

#include "Tools.h"
#include "Print.h"
#include "Header.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			EXPORT DataItem::BaseData& GetBase(wstring nickname)
			{
				return Data::getBase(nickname);
			}

			EXPORT DataItem::SystemData& GetSystem(wstring nickname)
			{
				return Data::getSystem(nickname);
			}

			EXPORT DataItem::ShipData& GetShip(wstring nickname)
			{
				return Data::getShip(nickname);
			}

			EXPORT DataItem::FactionData& GetFaction(wstring nickname)
			{
				return Data::getFaction(nickname);
			}

			EXPORT DataItem::GoodsData& GetGoods(wstring nickname)
			{
				return Data::getGoods(nickname);
			}


			EXPORT string GetStoragePath(const char *name)
			{
				char profilePath[MAX_PATH];

				GetUserDataPath(profilePath);

				string dataRootDir = string(profilePath) + "\\Accts\\Rev";
				string dataDir = dataRootDir + "\\" + name;

				if (!createStorageDir(dataRootDir.c_str())) {
					return "";
				}

				if (!createStorageDir(dataDir.c_str())) {
					return "";
				}

				return dataDir;
			}

			namespace Console
			{
				EXPORT void PrintError(wstring prefix, wstring message)
				{
					Print::Error(message, prefix);
				}

				EXPORT void PrintError(string prefix, string message)
				{
					Print::Error(message, prefix);
				}

				EXPORT void PrintInfo(wstring prefix, wstring message)
				{
					Print::Info(message, prefix);
				}

				EXPORT void PrintInfo(string prefix, string message)
				{
					Print::Info(message, prefix);
				}

				EXPORT void PrintDebug(wstring prefix, wstring message)
				{
					Print::Debug(message, prefix);
				}

				EXPORT void PrintDebug(string prefix, string message)
				{
					Print::Debug(message, prefix);
				}
			}

			namespace Client
			{
				EXPORT Clients::Clients& Get()
				{
					return Clients::Clients::Get();
				}
			}
		}
	}
}