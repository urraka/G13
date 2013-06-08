#pragma once

#include "../Entities/Soldier.h"
#include "../../System/Clock.h"

#include <enet/enet.h>
#include <string>

namespace net
{
	// this Player class lives on the server side

	class Message;

	class Player
	{
	public:
		Player();
		~Player();


		enum State
		{
			Disconnected = 0,
			Joining,
			Dead,
			Alive,
			StateCount
		};

		struct SoldierState
		{
			static const uint16_t Size = 2*4 + 2*4 + 2; // pos + vel + graphics

			fixvec2 position;
			fixvec2 velocity;
			bool flipped;
			int animation;
			int frame;
		};

		struct Info
		{
			static const uint16_t MaxNickBytes = 32;
			static const uint16_t MaxSize = 1 + (MaxNickBytes + 1) + SoldierState::Size; // id & state + null terminated nick + soldier state

			int id;
			State state;
			char nickname[MaxNickBytes + 1];
			SoldierState soldierState;
		};

		void update(Time dt);
		void disconnect();
		bool connected() const;
		void onConnect(ENetPeer *peer);
		void onDisconnect();
		void onMessage(const Message *msg);

		ENetPeer *peer() const;
		State state() const;
		Info info() const;
		SoldierState soldierState() const;
		const char *nickname() const;
		fixvec2 position() const;

		int id;

	private:
		ENetPeer *peer_;
		State state_;
		ent::Soldier soldier_;
		std::string name_;
		uint32_t joinTick_;
		uint32_t lastTick_; // last simulated tick
		std::vector<uint8_t> inputs_;

		void reset();
	};
}

#include "DataStream.h"

net::DataWriter& operator<<(net::DataWriter & s, net::Player::SoldierState const & state);
net::DataReader& operator>>(net::DataReader & s, net::Player::SoldierState & state);
