#pragma once

#include <stdint.h>

namespace g13
{
	namespace net
	{
		static const int MaxPlayers         = 32;
		static const int MaxHealth          = UINT16_MAX;
		static const int MaxTickOffset      = 63;
		static const int InterpolationTicks = 4;
		static const int TimeoutTicks       = 200;
		static const int CountdownTicks     = 40;
		static const int ChatTime           = 6000; // ms

		static const size_t MaxNickLength      = 20;
		static const size_t MinNickLength      = 1;

		enum Channels
		{
			ReliableChannel = 0,
			UnsequencedChannel,
			ChatChannel,
			ChannelCount
		};
	}
}
