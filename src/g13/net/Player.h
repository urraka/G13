#pragma once

#include <g13/g13.h>
#include <g13/math.h>
#include <g13/ent/Soldier.h>
#include <g13/ent/Bullet.h>
#include <gfx/Color.h>
#include <vector>
#include <string>

#include "constants.h"

namespace g13 {
namespace net {

class Player
{
public:
	Player();

	// enums

	enum State
	{
		Disconnected,
		Connecting,
		Spectator,
		Playing
	};

	// methods

	void initialize();
	bool connected() { return state == Spectator || state == Playing; }
	void updateBullets(Time dt, const coll::World &world);

	// member variables

	int id;
	std::string nickname;
	gfx::Color color;

	State state;
	int health;
	ent::Soldier soldier;
	std::vector<ent::Bullet> bullets;
};

}} // g13::net
