#pragma once

#include <stdint.h>

namespace net
{
	class Message;

	class PlayerLeaveMessage
	{
	public:
		static bool validate(const Message *msg);
		void serialize(Message *msg);
		void unserialize(const Message *msg);

		uint8_t id;

	private:
		uint8_t data_[1 + 1]; // msg type + id
	};
}
