#include "Soldier.h"
#include <g13/vars.h>
#include <g13/ent/Bullet.h>
#include <g13/cmp/BulletParams.h>
#include <g13/coll/World.h>

namespace g13 {
namespace ent {

Soldier::Soldier()
	:	shootingTime_(0)
{
	reset(fixvec2(0, 0));
}

void Soldier::update(Time dt, const coll::World &world, const cmp::SoldierInput &input)
{
	// update rope

	if (input.rope)
	{
		if (rope.idle() && canShootRope_)
		{
			const fixed a = input.computeAngle();
			rope.shoot(bulletSpawnPoint(physics.position, a), fpm::from_polar(a, vars::RopeVel));
			canShootRope_ = false;
		}
	}
	else
	{
		canShootRope_ = true;
		rope.pull();
	}

	rope.update(dt, *this, world);

	// update soldier

	switch (physics.mode)
	{
		case cmp::SoldierPhysics::Normal: if ( rope.hooked()) physics.hook(rope.position); break;
		case cmp::SoldierPhysics::Rope:   if (!rope.hooked()) physics.unhook();            break;
	}

	physics.update(dt, world, input);
	updateState(input);
	graphics.update(dt, state_);

	// update weapon

	if (input.shoot)
	{
		const Time rate = sys::time<sys::Milliseconds>(200);

		if (shootingTime_ == 0)
			shootingTime_ = rate;

		if (shootingTime_ >= rate)
		{
			shootingTime_ -= rate;

			const fixed angle = input.computeAngle();
			const fixvec2 &position = physics.position;

			fixvec2 spawnPoint = bulletSpawnPoint(position, angle);

			if (!world.collision(position + bodyOffset(), spawnPoint, fixrect(0, 0, 1, 1)))
			{
				cmp::BulletParams params(id, spawnPoint, 2000, angle);
				createBulletCallback.fire(&params);
			}
		}

		shootingTime_ += dt;
	}
	else
	{
		shootingTime_ = 0;
	}
}

fixvec2 Soldier::bulletSpawnPoint(const fixvec2 &position, const fixed &angle) const
{
	// All constants here are taken from SoldierGraphics, multiplied by the scale factor 0.15
	// and then multiplied by 65536 to get the fixed point value.
	// This should be handled by some Weapon component instead, with parameters, etc..

	// calculate distance from weapon center to bullet start point

	const fixed distances[] = {
		fpm::from_value(3932160),
		fpm::from_value(2949120),
		fpm::from_value(2703360),
		fpm::from_value(2703360),
		fpm::from_value(2457600)
	};

	fixed alpha = fpm::degrees(angle);

	alpha = alpha < -90 ? fixed(-180) - alpha :
	        alpha >  90 ? fixed( 180) - alpha :
	        alpha;

	const fixed theta = fixed(180) / fixed((int)countof(distances) - 1);
	const fixed index = (alpha + fixed(90)) / theta;
	const int a = fpm::to_int(fpm::floor(index));
	const int b = fpm::to_int(fpm::ceil(index));
	const fixed percent = (alpha - (fixed(a) * theta - fixed(90))) / theta;
	const fixed weapdist = fpm::lerp(distances[a], distances[b], percent);

	// calculate the bullet start point

	const fixvec2 direction = fixvec2(fpm::cos(angle), fpm::sin(angle));

	return fixvec2(position + bodyOffset() + direction * weapdist);
}

fixvec2 Soldier::bodyOffset() const
{
	return fixvec2(0, -fpm::from_value(1720320));
}

void Soldier::reset(fixvec2 pos)
{
	canShootRope_ = true;
	physics.reset(pos);
	updateState(cmp::SoldierInput());
	graphics.update(0, state_);
}

const cmp::SoldierState &Soldier::state() const
{
	return state_;
}

void Soldier::updateState(const cmp::SoldierInput &input)
{
	state_.position = physics.position;
	state_.velocity = physics.velocity;
	state_.duck  = physics.ducking();
	state_.floor = physics.floor();
	state_.hooked = !rope.idle();
	state_.hook = rope.idle() ? physics.position : rope.position;

	state_.rightwards = input.rightwards;
	state_.angle = input.angle;
}

}} // g13::ent
