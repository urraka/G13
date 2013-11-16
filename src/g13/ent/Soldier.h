#pragma once

#include <g13/g13.h>
#include <g13/math.h>
#include <g13/callback.h>
#include <g13/cmp/SoldierInput.h>
#include <g13/cmp/SoldierPhysics.h>
#include <g13/cmp/SoldierGraphics.h>
#include <g13/cmp/SoldierState.h>
#include <g13/coll/Entity.h>

namespace g13 {
namespace ent {

class Soldier
{
public:
	Soldier();

	void update(Time dt, const cmp::SoldierInput *inpt = 0);
	void reset(fixvec2 pos);
	void world(const coll::World *world);
	cmp::SoldierState state() const;

	cmp::SoldierInput input;
	cmp::SoldierPhysics physics;
	cmp::SoldierGraphics graphics;

	uint8_t id;

	Callback createBulletCallback;

	coll::Entity collisionEntity;

private:
	Time shootingTime_;
};

}} // g13::ent
