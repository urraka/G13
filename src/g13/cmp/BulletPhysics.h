#pragma once

#include <g13/g13.h>
#include <g13/math.h>

namespace g13 {
namespace cmp {

class BulletPhysics
{
public:
	bool update(Time dt, const coll::World *world);

	fixvec2 position;
	fixvec2 velocity;

	cbk::Callback *collisionCallback;
};

}} // g13::cmp
