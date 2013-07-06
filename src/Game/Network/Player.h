#pragma once

#include "../Entities/Soldier.h"

#include <hlp/ring.h>
#include <stdint.h>
#include <vector>
#include <enet/enet.h>

class Map;

namespace net {

class Player
{
public:
	enum State { Disconnected, Connecting, Connected, Playing };
	enum Mode  { Server, Local, Remote };

	enum
	{
		InvalidId     = 0xFF,
		MaxNameLength = 32,
		MinNameLength = 1,
		MaxTickOffset = 63
	};

	Player();

	void initialize();
	void update(Time dt, uint32_t tick);

	void onConnecting(ENetPeer *peer = 0);
	void onConnect(const char *name);
	void onDisconnect();
	void onJoin(uint32_t tick, const Map *map, const fixvec2 &position);
	void onSoldierState(uint32_t tick, const cmp::SoldierState &soldierState);
	void onInput(uint32_t tick, const cmp::SoldierInput &input);

	void mode(Mode mode);

	Mode        mode     () const;
	State       state    () const;
	bool        connected() const;
	uint8_t     id       () const;
	ENetPeer   *peer     () const;
	const char *name     () const;
	uint32_t    tick     () const;

	ent::Soldier *soldier();

private:
	uint8_t id_;
	State state_;
	Mode mode_;
	char name_[MaxNameLength + 1];
	ent::Soldier soldier_;
	uint32_t joinTick_;
	uint32_t lastInputTick_;
	std::vector<uint8_t> inputs_;
	uint32_t tick_;

	struct SoldierState
	{
		SoldierState() {}
		SoldierState(uint32_t t, const cmp::SoldierState &s) : tick(t), state(s) {}
		uint32_t tick;
		cmp::SoldierState state;
	};

	hlp::ring<SoldierState, 10> stateBuffer_;

	ENetPeer *peer_;
	Time connectTimeout_;

	friend class Multiplayer;
};

} // net
