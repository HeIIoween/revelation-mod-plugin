#include "Language.h"

#include "../flhookplugin_sdk/headers/FLHook.h"

#include "Tools.h"
#include "Print.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			Language* Language::Get()
			{
				static Language Instance;

				return &Instance;
			}

			Language::Language()
			{
			}

			Language::~Language()
			{
			}

			void Language::import(const char* path)
			{
				INI_Reader ini;
				string key = "";
				wstring val = L"";

				if (!ini.open(path, false))
				{
					return;
				}

				while (ini.read_header())
				{
					if (ini.is_header("Languages"))
					{
						while (ini.read_value())
						{
							if (ini.is_value("String"))
							{
								if (parse(ini.get_value_string(), key, val))
								{
									if (languages.find(key.c_str()) != languages.end())
									{
										Print::Info("Language string \"" + key + "\" already been defined. It will be overwrited.", "");
									}
									else
									{
										Print::Debug("Adding language string \"" + key + "\" ...", "");
									}

									languages[key.c_str()] = val;
								}
							}
						}
					}
				}

				ini.close();
			}

			const wstring& Language::lang(const string key, const wstring defaultStr)
			{
				if (languages.find(key) == languages.end())
				{
					languages[key] = defaultStr;

					Print::Debug(string("Language string \"").append(key).append("\" was not defined, using default instead."), "");
				}

				return languages[key];
			}

			bool Language::parse(string input, string &key, wstring &val)
			{
				int cutPos = 0;

				if ((cutPos = input.find_first_of(",")) == string::npos)
				{
					return false;
				}

				key = input.substr(0, cutPos);
				val = stows(input.substr(cutPos + 1, input.length()));

				trim(key);
				trim(val);

				return true;
			}


		}
	}
}