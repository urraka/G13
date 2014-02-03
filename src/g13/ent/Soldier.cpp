#include "Soldier.h"
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
	physics.update(dt, world, input);
	updateState(input);
	graphics.update(dt, state_);

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
		fixed::from_value(3932160),
		fixed::from_value(2949120),
		fixed::from_value(2703360),
		fixed::from_value(2703360),
		fixed::from_value(2457600)
	};

	fixed alpha = fpm::degrees(angle);

	alpha = alpha < -90 ? fixed(-180) - alpha :
	        alpha >  90 ? fixed( 180) - alpha :
	        alpha;

	const fixed theta = fixed(180) / fixed((int)countof(distances) - 1);
	const fixed index = (alpha + fixed(90)) / theta;
	const int a = fpm::floor(index).to_int();
	const int b = fpm::ceil(index).to_int();
	const fixed percent = (alpha - (fixed(a) * theta - fixed(90))) / theta;
	const fixed weapdist = fpm::lerp(distances[a], distances[b], percent);

	// calculate the bullet start point

	const fixvec2 direction = fixvec2(fpm::cos(angle), fpm::sin(angle));

	return fixvec2(position + bodyOffset() + direction * weapdist);
}

fixvec2 Soldier::bodyOffset() const
{
	return fixvec2(0, -fixed::from_value(1720320));
}

void Soldier::reset(fixvec2 pos)
{
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

	state_.rightwards = input.rightwards;
	state_.angle = input.angle;
}

}} // g13::ent
