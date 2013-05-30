#pragma once

#include "../Player.h"
#include "../Server.h"
#include "../../../Math/math.h"
#include <stdint.h>

namespace net
{
	class Message;

	class StartMessage
	{
	public:
		static bool validate(const Message *msg);
		void serialize(Message *msg);
		void unserialize(const Message *msg);

		Player::Info players[Server::MaxPeers];
		int playersCount;

	private:
		uint8_t data_[1 + Player::Info::MaxSize * Server::MaxPeers];
	};
}
