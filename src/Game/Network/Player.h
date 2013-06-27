#pragma once

#include "../Entities/Soldier.h"

#include <stdint.h>
#include <string>

class Map;

namespace net
{
	class Player
	{
	public:
		enum State { Disconnected, Connecting, Connected, Playing };

		Player();

		void initialize();
		void update(Time dt, uint32_t tick);

		void onConnecting();
		void onConnect(const char *name);
		void onDisconnect();
		void onJoin(uint32_t tick, const Map *map, const fixvec2 &position);
		void onSoldierState(uint32_t tick, const ent::Soldier::State &soldierState);
		void onInput(uint32_t tick, const cmp::SoldierInput &input);

		State state    () const;
		bool  connected() const;

		static const size_t MaxNameLength = 32;
		static const size_t MinNameLength = 1;

	private:
		uint8_t id_;
		State state_;
		char name_[MaxNameLength + 1];
		ent::Soldier soldier_;
		uint32_t joinTick_;
		uint32_t lastTick_;

		friend class Multiplayer;
	};
}
