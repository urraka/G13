#pragma once

#include <g13/g13.h>
#include <g13/Collision.h>
#include <g13/cmp/SoldierInput.h>
#include <g13/cmp/SoldierPhysics.h>
#include <g13/cmp/SoldierGraphics.h>
#include <g13/cmp/SoldierState.h>

#include "../../Math/math.h"

namespace g13 {
namespace ent {

class Soldier
{
public:
	Soldier();

	void update(Time dt, cmp::SoldierInput *inpt = 0);
	void reset(fixvec2 pos);
	void map(const Collision::Map *map);

	cmp::SoldierInput input;
	cmp::SoldierPhysics physics;
	cmp::SoldierGraphics graphics;

	cmp::SoldierState state();

private:
	cmp::SoldierState state_;
};

}} // g13::ent
