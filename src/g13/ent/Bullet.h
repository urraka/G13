#pragma once

#include <g13/g13.h>
#include <g13/math.h>
#include <g13/cmp/BulletPhysics.h>
#include <g13/cmp/BulletGraphics.h>

namespace g13 {
namespace ent {

class Bullet
{
public:
	enum State { Alive, Impact, Dead };

	void update(Time dt);
	void spawn(const Collision::Map *map, fixvec2 position, fixed speed, fixed angle);

	cmp::BulletPhysics physics;
	cmp::BulletGraphics graphics;

	State state;
};

}} // g13::ent
