#pragma once

#include "../flhookplugin_sdk/headers/FLHook.h"
#include "../flhookplugin_sdk/headers/plugin.h"

#define REVELATION_HOST_PLUGIN_CALL_LEVEL 255
#define REVELATION_SUB_PLUGIN_CALL_LEVEL 128
#define REVELATION_CMD_PREFIX Language::Get()->lang("DEFAULT_CMD_PREFIX", L"Revelation")

using namespace std;

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			extern PLUGIN_RETURNCODE PluginReturnCode;
		}
	}
}
