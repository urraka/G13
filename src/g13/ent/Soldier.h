#pragma once

#include <g13/g13.h>
#include <g13/math.h>
#include <g13/callback.h>
#include <g13/cmp/Rope.h>
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

	void update(Time dt, const coll::World &world, const cmp::SoldierInput &input);

	void reset(fixvec2 pos);
	const cmp::SoldierState &state() const;
	fixvec2 bodyOffset() const;
	fixvec2 center() const { return physics.bounds().center() + physics.position; }

	cmp::SoldierPhysics physics;
	cmp::SoldierGraphics graphics;
	cmp::Rope rope;

	uint8_t id;

	Callback createBulletCallback;

	coll::Entity entity;

private:
	Time shootingTime_;
	cmp::SoldierState state_;
	bool canShootRope_;

	fixvec2 bulletSpawnPoint(const fixvec2 &position, const fixed &angle) const;
	void updateState(const cmp::SoldierInput &input);
};

}} // g13::ent
