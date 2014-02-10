#pragma once

#include <g13/g13.h>
#include <g13/cmp/SoldierInput.h>
#include <hlp/ring.h>
#include <deque>

#include "Peer.h"
#include "Player.h"
#include "Ticked.h"

struct ENetPeer;

namespace g13 {
namespace net {

class ServerPlayer : public Player
{
public:
	// methods

	void initialize();
	void update(Time dt, int serverTick, coll::World &world);
	const coll::Entity *entity(int tick);

	// member variables

	typedef std::deque<Ticked<cmp::SoldierInput> > InputQueue;
	typedef hlp::ring<fixrect, 40> BoundsBuffer;

	Peer peer;

	int tick;
	int connectTick;
	int disconnectCountdown; // unset = -1;
	int inputTick;

	InputQueue inputs;

	int boundsbufTick; // base tick?
	BoundsBuffer boundsbuf;

	int pingTick;
	int pongTick;
};

}} // g13::net
