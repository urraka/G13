#pragma once

#include <g13/g13.h>
#include <g13/math.h>
#include <g13/callback.h>

namespace g13 {
namespace cmp {

class BulletPhysics
{
public:
	bool update(Time dt, const coll::World *world);

	fixvec2 position;
	fixvec2 velocity;

	Callback collisionCallback;
};

}} // g13::cmp
