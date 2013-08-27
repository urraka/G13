#include "Soldier.h"

namespace g13 {
namespace ent {

Soldier::Soldier()
{
	physics.input = &input;

	reset(fixvec2(0, 0));
}

void Soldier::update(Time dt, const cmp::SoldierInput *inpt)
{
	if (inpt != 0)
		input = *inpt;

	physics.update(dt);
	graphics.update(dt, state());

	if (input.shoot && bullet.state != Bullet::Alive)
	{
		const fixed value = fixed::from_value((int32_t)input.angle);
		const fixed maxValue = fixed::from_value(((1U << 16) - 1));
		const fixed &pi = fixed::pi;

		fixed angle = pi * value / maxValue - pi / fixed(2);

		if (!input.rightwards)
			angle = -angle + pi;

		fixvec2 offset = fixvec2(0, fixed(-26.25));
		offset += fixvec2(fpm::cos(angle), fpm::sin(angle)) * fixed(100);

		bullet.spawn(physics.map, physics.position + offset, 1500, angle);
	}

	bullet.update(dt);
}

void Soldier::reset(fixvec2 pos)
{
	physics.reset(pos);
	graphics.position.set(from_fixed(pos));

	input.rightwards = true;
	input.angle = 1U << 15;

	bullet.state = Bullet::Dead;
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

	return state_;
}

}} // g13::ent
