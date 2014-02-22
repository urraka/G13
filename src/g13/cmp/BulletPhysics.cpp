#include "BulletPhysics.h"
#include <g13/coll/World.h>
#include <g13/vars.h>

namespace g13 {
namespace cmp {

void BulletPhysics::update(Time dt, const coll::World &world)
{
	const fixed dts = fixed((int)dt / 1000) / fixed(1000);
	const fixrect bbox(0, 0, 1, 1);

	velocity += fixvec2(0, vars::Gravity) * dts;

	fixvec2 delta = velocity * dts;

	collision = world.collision(position, position + delta, bbox, coll::All);
	position = collision.position;
}

}} // g13::cmp
