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

	Bullet();
	Bullet(const cmp::BulletParams &params);

	void update(Time dt, const coll::World *world);

	cmp::BulletPhysics physics;
	cmp::BulletGraphics graphics;

	State state;
	uint8_t id;

	Callback collisionCallback;
};

}} // g13::ent
