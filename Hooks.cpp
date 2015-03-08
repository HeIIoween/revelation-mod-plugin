#include "Hooks.h"

#include "Client.h"
#include "Header.h"

using namespace raincious::FLHookPlugin::Revelation;

namespace HkIServerImpl
{
	static int timerSkips = 0, timerSkiper = 10;
	static uint onlinePlayers = 0;

	void __stdcall Timer()
	{
		PluginReturnCode = DEFAULT_RETURNCODE;

		if ((++timerSkips % timerSkiper) == 0)
		{
			Clients::Clients::Get().renew();

			timerSkips = 0;

			onlinePlayers = Clients::Clients::Get().size();

			if (onlinePlayers > 10)
			{
				timerSkiper = onlinePlayers;
			}
		}
	}

	void __stdcall SystemSwitchOutComplete(uint iShip, uint iClientID)
	{
		PluginReturnCode = DEFAULT_RETURNCODE;

		Clients::Clients::Get().renew(iClientID);
	}

	void __stdcall BaseEnter_AFTER(uint iBaseID, uint iClientID)
	{
		PluginReturnCode = DEFAULT_RETURNCODE;

		Clients::Clients::Get().renew(iClientID);
	}

	void __stdcall PlayerLaunch_AFTER(uint iShip, uint iClientID)
	{
		PluginReturnCode = DEFAULT_RETURNCODE;

		Clients::Clients::Get().renew(iClientID);
	}
	
	void __stdcall DisConnect(uint iClientID, enum EFLConnection p2)
	{
		PluginReturnCode = DEFAULT_RETURNCODE;

		Clients::Clients::Get().remove(iClientID);
	}
}