#pragma once

#include "../Player.h"
#include "../Server.h"
#include <stdint.h>

namespace net
{
	class Message;

	class GameStateMessage
	{
	public:
		static bool validate(const Message *msg);
		void serialize(Message *msg);
		void unserialize(const Message *msg);

		struct State
		{
			uint8_t id;
			Player::SoldierState state;
		};

		uint32_t tick;
		int nPlayers;
		State players[Server::MaxPeers];

	private:
		uint8_t data_[1 + 32 + (1 + Player::SoldierState::Size) * Server::MaxPeers]; // msg type + tick + array of (id + SoldierState)
	};
}
