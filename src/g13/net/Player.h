#pragma once

#include <g13/g13.h>
#include <g13/ent/Soldier.h>
#include <g13/ent/Bullet.h>
#include <g13/cmp/BulletParams.h>

#include <hlp/ring.h>
#include <stdint.h>
#include <vector>
#include <deque>
#include <enet/enet.h>

#include "Ticked.h"

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
		Playing,
		Dead
	};

	enum
	{
		InvalidId     = 0xFF,
		MaxNameLength = 20,
		MinNameLength = 1,
		MaxTickOffset = 63,
		MaxHealth     = UINT16_MAX
	};

	Player();

	void initialize();
	void updateLocal(Time dt);
	void updateRemote(Time dt, int tick);
	void updateServer(Time dt, int tick);
	void updateBullets(Time dt);
	void createBullet(const cmp::BulletParams &params, Callback collisionCallback);
	void setCollisionTick(int tick);

	void onConnecting(ENetPeer *peer = 0);
	void onConnect(const char *name);
	void onDisconnect(int tick);
	void onJoin(int tick, const Map *map, const fixvec2 &position);
	void onSoldierState(int tick, const cmp::SoldierState &soldierState);
	void onInput(int tick, const cmp::SoldierInput &input);
	void onBulletCreated(int tick, const cmp::BulletParams &params);
	void onDamage(int tick, int amount);

	State       state    () const;
	bool        connected() const;
	uint8_t     id       () const;
	ENetPeer   *peer     () const;
	const char *name     () const;
	int         tick     () const;
	int         health   () const;

	ent::Soldier *soldier();

	std::vector<ent::Bullet> &bullets() { return bullets_; }

private:
	typedef Ticked<cmp::BulletParams> BulletParams;
	typedef Ticked<cmp::SoldierState> SoldierState;
	typedef Ticked<cmp::SoldierInput> SoldierInput;

	uint8_t id_;
	State state_;
	char name_[MaxNameLength * 4 + 1];
	ent::Soldier soldier_;
	int joinTick_;
	int disconnectTick_;
	int lastInputTick_;
	std::vector<SoldierInput> inputs_;
	int tick_;
	hlp::ring<SoldierState, 10> stateBuffer_;
	ENetPeer *peer_;
	Time connectTimeout_;
	int health_;

	std::deque<BulletParams> bulletsQueue_;
	std::vector<ent::Bullet> bullets_;

	int boundsBufferTick_;
	hlp::ring<fixrect, 40> boundsBuffer_;

	friend class Multiplayer;

	#ifdef DEBUG
		friend class g13::Debugger;
	#endif
};

}} // g13::net
