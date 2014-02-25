#include "Rope.h"
#include <g13/vars.h>
#include <g13/ent/Soldier.h>
#include <g13/coll/World.h>
#include <g13/coll/Result.h>

namespace g13 {
namespace ent {

void Rope::update(Time dt, const Soldier &soldier, const coll::World &world)
{
	static const fixrect bbox(0, 0, 1, 1);

	switch (state)
	{
		case Throwing:
		{
			fixvec2 delta = velocity * to_fixed(dt);

			const fixvec2 center = soldier.physics.position;
			const fixvec2 relpos = (position + delta) - center;
			const fixed   L      = fpm::length(relpos);

			if (L > vars::MaxRopeLength)
			{
				state = Pulling;
				delta = (center + (relpos / L) * vars::MaxRopeLength) - position;
			}

			coll::Result collision = world.collision(position, position + delta, bbox);
			position = collision.position;

			if (collision.segment)
				state = Hooked;
		}
		break;

		case Pulling:
		{
			state = Idle;
		}
		break;

		default: return;
	}
}

void Rope::shoot(const fixvec2 &spawnpoint, const fixvec2 &vel)
{
	state = Throwing;
	position = spawnpoint;
	velocity = vel;
}

void Rope::pull()
{
	if (state != Idle)
		state = Pulling;
}

}} // g13::ent
