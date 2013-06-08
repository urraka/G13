#pragma once

#include <stdint.h>

namespace net
{
	class InputMessage
	{
	public:
		static bool validate(const Message *msg);
		void serialize(Message *msg);
		void unserialize(const Message *msg);

		uint32_t tick;
		uint8_t input;

	private:
		uint8_t data_[1 + 32 + 1]; // msg type + tick + input
	};
}
