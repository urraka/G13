#include "BulletPhysics.h"
#include <g13/coll/collision.h>

namespace g13 {
namespace cmp {

bool BulletPhysics::update(Time dt, const coll::World *world)
{
	const fixed dts = fixed((int)dt / 1000) / fixed(1000);
	const fixed kGravity = world->gravity();
	const fixrect bbox(0, 0, 1, 1);

	velocity += fixvec2(0, kGravity) * dts;

	fixvec2 delta = velocity * dts;

	coll::Result collision = world->collision(position, position + delta, bbox, coll::All);

	position = collision.position;

	if (collision.entity != 0)
		collisionCallback.fire(0);

	return collision.collided();
}

}} // g13::cmp
