#include "RevelationVersionCommand.h"

#include "Client.h"

#include "Print.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			namespace Commands
			{

				RevelationVersionCommand::RevelationVersionCommand()
				{
				}


				RevelationVersionCommand::~RevelationVersionCommand()
				{
				}

				bool RevelationVersionCommand::execute(shared_ptr<Clients::Client> &client, vector <wstring> &parameters)
				{
					Clients::Client::MessageAssign assign;

					assign[L"CLIENT_ID"] = to_wstring(client->ID());
					assign[L"CLIENT_IP"] = client->getIP();
					assign[L"PLAYER_NAME"] = client->getName();
					assign[L"MOD_AUTHOR"] = L"NeXoSE <nexose@gmail.com>";

					client->sendXMLMessage(L"<TRA color=\"#0099ff\" font=\"4\" bold=\"default\" underline=\"default\"/><TEXT>Revelation Mod</TEXT>");
					client->sendXMLMessage(L"<TRA color=\"aqua\" font=\"default\" bold=\"default\"/><TEXT>(C) 2010 - 2015 ${MOD_AUTHOR}.</TEXT>", assign);
					client->sendXMLMessage(
						L"<TRA color=\"aqua\" font=\"default\" bold=\"default\"/><TEXT>For more information, updates and How To guide, please visit: </TEXT>" \
						L"<TRA color=\"#33cc66\" font=\"default\" underline=\"true\" bold=\"default\"/><TEXT>http://www.3ax.org/work/revelationmod-origin</TEXT>" \
						L"<TRA color=\"aqua\" font=\"default\" underline=\"default\" bold=\"default\"/><TEXT>. Enjoy! :D</TEXT>"
						);
					client->sendXMLMessage(L"");

					client->sendXMLMessage(L"<TRA color=\"#ffcc33\" font=\"2\" bold=\"default\"/><TEXT>Client Information</TEXT>");
					client->sendXMLMessage(L"<TRA color=\"#ff6600\" font=\"default\" bold=\"default\"/><TEXT>Client ID: ${CLIENT_ID}</TEXT>", assign);
					client->sendXMLMessage(L"<TRA color=\"#ff6600\" font=\"default\" bold=\"default\"/><TEXT>IP Address: ${CLIENT_IP}</TEXT>", assign);
					client->sendXMLMessage(L"<TRA color=\"#ff6600\" font=\"default\" bold=\"default\"/><TEXT>Character: ${PLAYER_NAME}</TEXT>", assign);
				
					return true;
				}

			}
		}
	}
}
