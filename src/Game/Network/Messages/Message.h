#pragma once

#include <stdint.h>

namespace net
{
	class Message
	{
	public:
		Message(const uint8_t *data = 0, uint16_t length = 0);

		bool validate();
		uint8_t type() const;

		enum Type
		{
			Nick = 0,
			SpawnRequest,
			Input,
			Start,
			PlayerJoin,
			PlayerLeave,
			Spawn,
			GameState,
			TypeCount
		};

		const uint8_t *data; // first byte is always message type
		uint16_t length;
	};
}
