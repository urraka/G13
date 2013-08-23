#include "Soldier.h"

namespace g13 {
namespace ent {

Soldier::Soldier()
{
	physics.input = &input;
	input.rightwards = true;
	input.angle = 1U << 15;
}

void Soldier::update(Time dt, const cmp::SoldierInput *inpt)
{
	if (inpt != 0)
		input = *inpt;

	physics.update(dt);
	graphics.update(dt, state());
}

void Soldier::reset(fixvec2 pos)
{
	physics.reset(pos);
	graphics.position.set(from_fixed(pos));

	input.rightwards = true;
	input.angle = 1U << 15;
}

void Soldier::map(const Collision::Map *map)
{
	physics.map = map;
}

cmp::SoldierState Soldier::state()
{
	state_.position = physics.position;
	state_.velocity = physics.velocity;
	state_.duck  = physics.ducking();
	state_.floor = physics.floor();

	state_.rightwards = input.rightwards;
	state_.angle = input.angle;

	// if (input.right) state_.flip = true;
	// if (input.left) state_.flip = false;

	return state_;
}

}} // g13::ent
