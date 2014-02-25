#pragma once

#include <g13/g13.h>
#include <g13/math.h>

namespace g13 {
namespace ent {

class Rope
{
public:
	Rope() : state(Idle) {}

	enum State { Idle, Throwing, Hooked, Pulling };

	void update(Time dt, const Soldier &soldier, const coll::World &world);
	void shoot(const fixvec2 &spawnpoint, const fixvec2 &velocity);
	void pull();

	bool idle() const { return state == Idle; }
	bool hooked() const { return state == Hooked; }

	State state;
	fixvec2 position;
	fixvec2 velocity;
};

}} // g13::ent
