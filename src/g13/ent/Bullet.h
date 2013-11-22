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
	Bullet(const cmp::BulletParams &params, coll::Entity *ownerEntity);

	void update(Time dt, const coll::World *world);

	cmp::BulletPhysics physics;
	cmp::BulletGraphics graphics;

	State state;
	uint8_t id;

	Callback collisionCallback;

private:
	coll::Entity *ownerEntity_;
};

}} // g13::ent
