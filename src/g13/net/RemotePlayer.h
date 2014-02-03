#pragma once

#include <g13/g13.h>
#include <g13/cmp/BulletParams.h>
#include <hlp/ring.h>
#include <vector>

#include "Player.h"
#include "Ticked.h"

namespace g13 {
namespace net {

class RemotePlayer : public Player
{
public:
	RemotePlayer();

	// methods

	void initialize();
	void update(Time dt, int clientTick, coll::World &world);
	void onBullet(int tick, const cmp::BulletParams &params);

	// member variables

	typedef hlp::ring<Ticked<cmp::SoldierState>, 10> StateBuffer;
	typedef std::vector< Ticked<cmp::BulletParams> > BulletBuffer;

	int connectTick; // loginTick?
	int spawnTick;

	StateBuffer  stateBuffer;
	BulletBuffer bulletBuffer[2];

private:
	void fireBullets(int tick);
};

}} // g13::net
