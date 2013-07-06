#pragma once

#include "../../Math/math.h"
#include "../../System/Clock.h"
#include "../Collision.h"
#include "../Components/SoldierInput.h"
#include "../Components/SoldierPhysics.h"
#include "../Components/SoldierGraphics.h"
#include "../Components/SoldierState.h"

class Replay;

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

} // ent
