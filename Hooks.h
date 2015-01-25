#include "../flhookplugin_sdk/headers/FLHook.h"

namespace HkIServerImpl
{
	void __stdcall Timer();

	void __stdcall SystemSwitchOutComplete(uint iShip, uint iClientID);

	void __stdcall BaseEnter(uint iBaseID, uint iClientID);

	void __stdcall PlayerLaunch(uint iShip, uint iClientID);

	void __stdcall DisConnect(uint iClientID, enum EFLConnection p2);
}