#include "Bullet.h"
#include <g13/cmp/BulletParams.h>
#include <g13/coll/World.h>

namespace g13 {
namespace ent {

Bullet::Bullet() {}

Bullet::Bullet(const cmp::BulletParams &params, coll::Entity *ownerEntity)
	:	hitsOwner_(false)
{
	state = Alive;
	id = params.playerid;

	physics.position = params.position;
	physics.velocity = fixvec2(fpm::cos(params.angle), fpm::sin(params.angle)) * params.speed;

	graphics.initialPosition = from_fixed(params.position);
	graphics.position.set(graphics.initialPosition);
	graphics.angle.set(params.angle.to_float());

	ownerEntity_ = ownerEntity;
}

void Bullet::update(Time dt, const coll::World &world)
{
	if (state == Impact)
		state = Dead;

	if (state == Dead)
		return;

	if (!hitsOwner_)
	{
		fixrect bounds = fpm::expand(ownerEntity_->previous, ownerEntity_->current);

		if (!fpm::intersects(bounds, fixrect(0, 0, 1, 1) + physics.position))
			hitsOwner_ = true;
	}

	fixvec2 prevPosition = physics.position;

	bool active = ownerEntity_->active;
	ownerEntity_->active = hitsOwner_;

	physics.update(dt, world);

	ownerEntity_->active = active;

	const coll::Result &collision = physics.collision;

	if (collision.collided())
	{
		if (collision.entity != 0)
		{
			struct params_t
			{
				uint8_t bulletOwner;
				const coll::Entity *entity;
			};

			params_t params = {id, collision.entity};

			collisionCallback.fire(&params);
		}

		if (fpm::length2(physics.position - prevPosition) > 1)
			state = Impact;
		else
			state = Dead;
	}

	if (!fpm::contains(world.bounds(), physics.position))
		state = Dead;

	graphics.update(dt, physics);
}

}} // g13::ent
