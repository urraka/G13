#pragma once

#include <g13/g13.h>
#include <g13/math.h>
#include <g13/Collision.h>
#include <g13/cmp/SoldierInput.h>
#include <g13/cmp/SoldierPhysics.h>
#include <g13/cmp/SoldierGraphics.h>
#include <g13/cmp/SoldierState.h>

namespace g13 {
namespace ent {

class Bullet;

class Soldier
{
public:
	Soldier();

	void update(Time dt, const cmp::SoldierInput *inpt = 0);
	void reset(fixvec2 pos);
	void map(const Collision::Map *map);
	cmp::SoldierState state() const;

	cmp::SoldierInput input;
	cmp::SoldierPhysics physics;
	cmp::SoldierGraphics graphics;

	typedef void (*CreateBulletCallback)(void*, uint8_t, const fixvec2&, const fixed&, const fixed&);

	uint8_t id;
	void *listener;
	CreateBulletCallback createBullet;

private:
	Time shootingTime_;
};

}} // g13::ent
