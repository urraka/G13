#include "Bullet.h"
#include <g13/cmp/BulletParams.h>
#include <g13/coll/collision.h>

namespace g13 {
namespace ent {

Bullet::Bullet() {}

Bullet::Bullet(const cmp::BulletParams &params)
{
	state = Alive;
	id = params.playerid;

	physics.position = params.position;
	physics.velocity = fixvec2(fpm::cos(params.angle), fpm::sin(params.angle)) * params.speed;

	graphics.initialPosition = from_fixed(params.position);
	graphics.position.set(graphics.initialPosition);
	graphics.angle.set(params.angle.to_float());
}

void Bullet::update(Time dt, const coll::World *world)
{
	if (state == Impact)
		state = Dead;

	if (state == Dead)
		return;

	fixvec2 prevPosition = physics.position;

	physics.update(dt, world);

	if (physics.collision.collided())
	{
		if (physics.collision.entity != 0)
		{
			struct params_t
			{
				uint8_t bulletOwner;
				const coll::Entity *entity;
			};

			params_t params = {id, physics.collision.entity};

			collisionCallback.fire(&params);
		}

		if (fpm::length2(physics.position - prevPosition) > 1)
			state = Impact;
		else
			state = Dead;
	}

	if (!fpm::contains(world->bounds(), physics.position))
		state = Dead;

	graphics.update(dt, &physics);
}

}} // g13::ent
