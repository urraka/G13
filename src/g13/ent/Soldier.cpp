#include "Soldier.h"
#include <g13/ent/Bullet.h>
#include <g13/cmp/BulletParams.h>

namespace g13 {
namespace ent {

Soldier::Soldier()
	:	createBullet(0),
		shootingTime_(0)
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

	if (input.shoot)
	{
		const Time rate = sys::time<sys::Milliseconds>(200);

		if (shootingTime_ == 0)
			shootingTime_ = rate;

		if (shootingTime_ >= rate)
		{
			shootingTime_ -= rate;

			const fixed value = fixed::from_value((int32_t)input.angle);
			const fixed maxValue = fixed::from_value(UINT16_MAX);
			const fixed &pi = fixed::pi;

			fixed angle = pi * value / maxValue - pi / fixed(2);

			if (!input.rightwards)
				angle = -angle + pi;

			fixvec2 position = physics.position + fixvec2(0, fixed(-26.25));
			position += fixvec2(fpm::cos(angle), fpm::sin(angle)) * fixed(80);

			createBullet(listener, cmp::BulletParams(id, position, 2000, angle));
		}

		shootingTime_ += dt;
	}
	else
	{
		shootingTime_ = 0;
	}
}

void Soldier::reset(fixvec2 pos)
{
	physics.reset(pos);
	graphics.position.set(from_fixed(pos));

	input.rightwards = true;
	input.angle = UINT16_MAX / 2;
}

void Soldier::world(const coll::World *world)
{
	physics.world = world;
}

cmp::SoldierState Soldier::state() const
{
	cmp::SoldierState state;

	state.position = physics.position;
	state.velocity = physics.velocity;
	state.duck  = physics.ducking();
	state.floor = physics.floor();

	state.rightwards = input.rightwards;
	state.angle = input.angle;

	return state;
}

}} // g13::ent
