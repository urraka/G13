#pragma once

#include <g13/g13.h>
#include <g13/math.h>
#include <g13/cmp/SoldierInput.h>
#include <g13/cmp/SoldierPhysics.h>
#include <g13/cmp/SoldierGraphics.h>
#include <g13/cmp/SoldierState.h>

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

	typedef void (*CreateBulletCallback)(void*, const cmp::BulletParams&);

	uint8_t id;
	void *listener;
	CreateBulletCallback createBullet;

private:
	Time shootingTime_;
};

}} // g13::ent
