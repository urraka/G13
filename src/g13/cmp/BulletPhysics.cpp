#include "BulletPhysics.h"

namespace g13 {
namespace cmp {

bool BulletPhysics::update(Time dt, const Collision::Map *map)
{
	const fixed dts = fixed((int)dt / 1000) / fixed(1000);
	const fixed kGravity = fixed(1470); // 9.8 * 150 - warning: duplicated in SoldierPhysics.cpp
	const fixrect bbox(0, 0, 1, 1);

	velocity += fixvec2(0, kGravity) * dts;

	fixvec2 delta = velocity * dts;

	Collision::Result collision = Collision::resolve(map, position, position + delta, bbox);

	position = collision.position;

	return collision.node != 0;
}

}} // g13::cmp
