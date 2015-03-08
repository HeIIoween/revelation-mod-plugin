#include "Export.h"

#include "Tools.h"
#include "Print.h"
#include "Header.h"
#include "Language.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			EXPORT DataItem::BaseData* GetBase(uint uid)
			{
				return Data::getBase(uid);
			}

			EXPORT DataItem::BaseData* GetBase(wstring nickname)
			{
				return Data::getBase(nickname);
			}

			EXPORT DataItem::SystemData* GetSystem(uint uid)
			{
				return Data::getSystem(uid);
			}

			EXPORT DataItem::SystemData* GetSystem(wstring nickname)
			{
				return Data::getSystem(nickname);
			}

			EXPORT DataItem::ShipData* GetShip(uint uid)
			{
				return Data::getShip(uid);
			}

			EXPORT DataItem::ShipData* GetShip(wstring nickname)
			{
				return Data::getShip(nickname);
			}

			EXPORT DataItem::FactionData* GetFaction(uint uid)
			{
				return Data::getFaction(uid);
			}

			EXPORT DataItem::FactionData* GetFaction(wstring nickname)
			{
				return Data::getFaction(nickname);
			}

			EXPORT DataItem::GoodsData* GetGoods(uint uid)
			{
				return Data::getGoods(uid);
			}

			EXPORT DataItem::GoodsData* GetGoods(wstring nickname)
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

			EXPORT int GetRandNumber(int min, int max)
			{
				return randNumber(min, max);
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

			namespace Cmd
			{
				EXPORT void Mount(const wstring& prefix, const wstring& paramater, const wstring& description, unique_ptr<CommandBase> commandProc)
				{
					Command::Get()->mount(prefix, paramater, description, move(commandProc));
				}

				EXPORT void Demount(const wstring& prefix)
				{
					Command::Get()->demount(prefix);
				}
			}

			namespace Lang
			{
				EXPORT const wstring& Get(const string key, const wstring defaultStr)
				{
					return Language::Get()->lang(key, defaultStr);
				}

				EXPORT void Import(const char* path)
				{
					Language::Get()->import(path);
				}
			}
		}
	}
}