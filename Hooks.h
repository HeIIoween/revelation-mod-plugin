#pragma once

#include "../flhookplugin_sdk/headers/FLHook.h"

namespace HkIServerImpl
{
	void __stdcall Timer();

	void __stdcall SystemSwitchOutComplete(uint iShip, uint iClientID);

	void __stdcall BaseEnter_AFTER(uint iBaseID, uint iClientID);

	void __stdcall PlayerLaunch_AFTER(uint iShip, uint iClientID);

	void __stdcall DisConnect_AFTER(uint iClientID, enum EFLConnection p2);
}