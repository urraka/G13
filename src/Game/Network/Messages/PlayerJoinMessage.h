#pragma once

#include "../Player.h"
#include <stdint.h>

namespace net
{
	class Message;

	class PlayerJoinMessage
	{
	public:
		static bool validate(const Message *msg);
		void serialize(Message *msg);
		void unserialize(const Message *msg);

		uint8_t id;
		char nickname[Player::Info::MaxNickBytes + 1]; // +1 for null

	private:
		uint8_t data_[1 + 1 + Player::Info::MaxNickBytes]; // msg type + id + nick (not null terminated)
	};
}
