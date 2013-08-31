#pragma once

#include <g13/g13.h>
#include <g13/ent/Soldier.h>
#include <g13/cmp/BulletParams.h>

#include <hlp/ring.h>
#include <stdint.h>
#include <vector>
#include <deque>
#include <enet/enet.h>

namespace g13 {
namespace net {

class Player
{
public:
	enum State
	{
		Disconnected,
		Connecting,
		Connected,
		Playing
	};

	enum
	{
		InvalidId     = 0xFF,
		MaxNameLength = 20,
		MinNameLength = 1,
		MaxTickOffset = 63
	};

	Player();

	void initialize();
	void updateLocal(Time dt);
	void updateRemote(Time dt, uint32_t tick);
	void updateServer(Time dt);

	void onConnecting(ENetPeer *peer = 0);
	void onConnect(const char *name);
	void onDisconnect(uint32_t tick);
	void onJoin(uint32_t tick, const Map *map, const fixvec2 &position);
	void onSoldierState(uint32_t tick, const cmp::SoldierState &soldierState);
	void onInput(uint32_t tick, const cmp::SoldierInput &input);
	void onBulletCreated(uint32_t tick, const cmp::BulletParams &params);

	State       state    () const;
	bool        connected() const;
	uint8_t     id       () const;
	ENetPeer   *peer     () const;
	const char *name     () const;
	uint32_t    tick     () const;

	ent::Soldier *soldier();

private:
	struct BulletInfo
	{
		uint32_t tick;
		cmp::BulletParams params;

		BulletInfo() {}
		BulletInfo(uint32_t t, const cmp::BulletParams &p) : tick(t), params(p) {}
	};

	struct SoldierState
	{
		SoldierState() {}
		SoldierState(uint32_t t, const cmp::SoldierState &s) : tick(t), state(s) {}
		uint32_t tick;
		cmp::SoldierState state;
	};

	uint8_t id_;
	State state_;
	char name_[MaxNameLength * 4 + 1];
	ent::Soldier soldier_;
	uint32_t joinTick_;
	uint32_t disconnectTick_;
	uint32_t lastInputTick_;
	std::vector<cmp::SoldierInput> inputs_;
	uint32_t tick_;
	hlp::ring<SoldierState, 10> stateBuffer_;
	ENetPeer *peer_;
	Time connectTimeout_;
	std::deque<BulletInfo> bullets_;

	friend class Multiplayer;
};

}} // g13::net
