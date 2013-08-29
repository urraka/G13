#pragma once

#include <g13/g13.h>
#include <g13/math.h>
#include <g13/Collision.h>

namespace g13 {
namespace cmp {

class BulletPhysics
{
public:
	bool update(Time dt, const Collision::Map *map);

	fixvec2 position;
	fixvec2 velocity;

	const Collision::Map *map;
};

}} // g13::cmp
