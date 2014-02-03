#pragma once

#include <g13/g13.h>
#include <g13/math.h>
#include <g13/callback.h>
#include <g13/coll/Result.h>

namespace g13 {
namespace cmp {

class BulletPhysics
{
public:
	void update(Time dt, const coll::World &world);

	fixvec2 position;
	fixvec2 velocity;
	coll::Result collision;
};

}} // g13::cmp
