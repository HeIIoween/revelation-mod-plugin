#include "Data.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{

			map <wstring, DataItem::BaseData> Data::baseData;
			map <wstring, DataItem::SystemData> Data::systemData;
			map <wstring, DataItem::ShipData> Data::shipData;
			map <wstring, DataItem::FactionData> Data::factionData;
			map <wstring, DataItem::GoodsData> Data::goodsData;

			Data::Data()
			{
			}

			Data::~Data()
			{
			}

			// Base
			void Data::addBase(DataItem::BaseData item)
			{
				wstring nick = item.getNickname();

				if (baseData.find(nick) != baseData.end()) {
					Exception::RedeclaredNickname e;

					e.nickname(nick);

					throw e;
				}

				baseData.insert(make_pair(nick, item));
			}

			DataItem::BaseData& Data::getBase(wstring nickname)
			{
				nickname = ToLower(nickname);

				if (baseData.find(nickname) == baseData.end()) {
					Exception::NickNameNotFound e;

					e.nickname(nickname);

					throw e;
				}

				return baseData[nickname];
			}

			map <wstring, DataItem::BaseData> Data::getAllBases()
			{
				return baseData;
			}

			// System
			void Data::addSystem(DataItem::SystemData item)
			{
				wstring nick = item.getNickname();

				if (systemData.find(nick) != systemData.end()) {
					Exception::RedeclaredNickname e;

					e.nickname(nick);

					throw e;
				}

				systemData.insert(make_pair(nick, item));
			}

			DataItem::SystemData& Data::getSystem(wstring nickname)
			{
				nickname = ToLower(nickname);

				if (systemData.find(nickname) == systemData.end()) {
					Exception::NickNameNotFound e;

					e.nickname(nickname);

					throw e;
				}

				return systemData[nickname];
			}

			// Ship
			void Data::addShip(DataItem::ShipData item)
			{
				wstring nick = item.getNickname();

				if (shipData.find(nick) != shipData.end()) {
					Exception::RedeclaredNickname e;

					e.nickname(nick);

					throw e;
				}

				shipData.insert(make_pair(nick, item));
			}

			DataItem::ShipData& Data::getShip(wstring nickname)
			{
				nickname = ToLower(nickname);

				if (shipData.find(nickname) == shipData.end()) {
					Exception::NickNameNotFound e;

					e.nickname(nickname);

					throw e;
				}

				return shipData[nickname];
			}

			// Faction
			void Data::addFaction(DataItem::FactionData item)
			{
				wstring nick = item.getNickname();

				if (factionData.find(nick) != factionData.end()) {
					Exception::RedeclaredNickname e;

					e.nickname(nick);

					throw e;
				}

				factionData.insert(make_pair(nick, item));
			}

			DataItem::FactionData& Data::getFaction(wstring nickname)
			{
				nickname = ToLower(nickname);

				if (factionData.find(nickname) == factionData.end()) {
					Exception::NickNameNotFound e;

					e.nickname(nickname);

					throw e;
				}

				return factionData[nickname];
			}

			// Goods
			void Data::addGoods(DataItem::GoodsData item)
			{
				wstring nick = item.getNickname();

				if (goodsData.find(nick) != goodsData.end()) {
					Exception::RedeclaredNickname e;

					e.nickname(nick);

					throw e;
				}

				goodsData.insert(make_pair(nick, item));
			}

			DataItem::GoodsData& Data::getGoods(wstring nickname)
			{
				nickname = ToLower(nickname);

				if (goodsData.find(nickname) == goodsData.end()) {
					Exception::NickNameNotFound e;

					e.nickname(nickname);

					throw e;
				}

				return goodsData[nickname];
			}
		}
	}
}