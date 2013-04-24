#include "../Game.h"
#include "SoldierPhysics.h"

SoldierPhysics::SoldierPhysics() : map(0), ducking_(false), currentNode_(0) {}

void SoldierPhysics::update(Time dt)
{
	const fixed dts = fixed((int)dt / 1000) / fixed(1000);
	const fixed G = fixed(1470); // 9.8 * 150
	const fixed walkVelocity = fixed(250);
	const fixed duckVelocity = fixed(150);
	const fixed runVelocity = fixed(300);

	acceleration.x = fixed::zero;
	acceleration.y = G;
	velocity.x = input.move == SoldierInput::Right ? walkVelocity : input.move == SoldierInput::Left ? -walkVelocity : fixed::zero;
	velocity.y += acceleration.y * dts;

	fixvec2 dest = position + velocity * dts;
	Collision::Result collision = Collision::resolve(*map, position, dest, bbox);
	position = collision.position;

	if (collision.node != 0)
		velocity.y = fixed::zero;
}

void SoldierPhysics::teleport(fixvec2 pos)
{
	position = pos;
	velocity.x = fixed::zero;
	velocity.y = fixed::zero;
	acceleration.x = fixed::zero;
	acceleration.y = fixed::zero;

	ducking_ = false;
	currentNode_ = 0;
}
