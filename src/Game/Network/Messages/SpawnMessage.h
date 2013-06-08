#pragma once

#include "../../../Math/math.h"
#include <stdint.h>

namespace net
{
	class SpawnMessage
	{
	public:
		static bool validate(const Message *msg);
		void serialize(Message *msg);
		void unserialize(const Message *msg);

		uint8_t id;
		uint32_t tick;
		fixvec2 position;

	private:
		uint8_t data_[1 + 1 + 4 + 4*2]; // msg type + id + tick + position
	};
}
