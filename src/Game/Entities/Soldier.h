#pragma once

#include "../../Math/math.h"
#include "../../System/Clock.h"
#include "../Collision.h"
#include "../Components/SoldierInput.h"
#include "../Components/SoldierPhysics.h"
#include "../Components/SoldierGraphics.h"

class Replay;

class Soldier
{
public:
	Soldier();

	void update(Time dt, Replay *replay);
	void reset(fixvec2 pos);
	void map(const Collision::Map *map);

	Sprite sprite;
	SoldierInput input;
	SoldierPhysics physics;
	SoldierGraphics graphics;
};
