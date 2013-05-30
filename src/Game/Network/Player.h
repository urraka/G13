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
			Playing,
			Dead
		};

		struct Info
		{
			static const uint16_t MaxNickBytes = 32;
			static const uint16_t MaxSize = 1 + (MaxNickBytes + 1) + 2*4 + 2*4 + 1;

			int id;
			State state;
			char nickname[MaxNickBytes + 1];
			bool flipped;
			int frame;
			fixvec2 position;
			fixvec2 velocity;
		};

		void update(Time dt);
		void disconnect();
		bool connected() const;
		void onConnect(ENetPeer *peer);
		void onDisconnect();
		void onMessage(const Message *msg);

		ENetPeer *peer() const;
		State state() const;

		size_t serialize(uint8_t *buffer) const;
		void info(Info *info);

		int id;

	private:
		ENetPeer *peer_;
		State state_;
		ent::Soldier soldier_;
		std::string name_;
		uint32_t joinTick_;
		uint32_t baseTick_;

		void reset();
	};
}
