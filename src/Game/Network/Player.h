#pragma once

#include "../Entities/Soldier.h"

#include <stdint.h>
#include <vector>
#include <enet/enet.h>

class Map;

namespace net
{
	class Player
	{
	public:
		enum State { Disconnected, Connecting, Connected, Playing };
		enum Mode { Server, Local, Remote };

		Player();

		void initialize();
		void update(Time dt, uint32_t tick);

		void onConnecting(ENetPeer *peer = 0);
		void onConnect(const char *name);
		void onDisconnect();
		void onJoin(uint32_t tick, const Map *map, const fixvec2 &position);
		void onSoldierState(uint32_t tick, const ent::Soldier::State &soldierState);
		void onInput(uint32_t tick, const cmp::SoldierInput &input);

		void mode(Mode mode);

		Mode        mode     () const;
		State       state    () const;
		bool        connected() const;
		uint8_t     id       () const;
		ENetPeer   *peer     () const;
		const char *name     () const;

		static const size_t MaxNameLength = 32;
		static const size_t MinNameLength = 1;

	private:
		uint8_t id_;
		State state_;
		Mode mode_;
		char name_[MaxNameLength + 1];
		ent::Soldier soldier_;
		uint32_t joinTick_;
		uint32_t lastInputTick_; // this could just go away i think

		struct SoldierState
		{
			SoldierState() : received(false) {}
			bool received;
			ent::Soldier::State state;
		};

		// remote players state is stored in a ring buffer to interpolate N ticks behind
		SoldierState stateBuffer_[5];
		int      stateBase_;
		uint32_t stateTick_;

		ENetPeer *peer_;
		Time connectTimeout_;

		friend class Multiplayer;
	};
}
