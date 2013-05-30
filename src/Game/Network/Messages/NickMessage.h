#pragma once

#include "../Player.h"
#include <stdint.h>

namespace net
{
	class Message;

	class NickMessage
	{
	public:
		static bool validate(const Message *msg);
		void serialize(Message *msg);
		void unserialize(const Message *msg);

		char nickname[Player::Info::MaxNickBytes + 1]; // +1 for null

	private:
		uint8_t data_[1 + Player::Info::MaxNickBytes]; // +1 for type
	};
}
