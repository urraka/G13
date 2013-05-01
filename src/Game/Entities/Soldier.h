#pragma once

#include "../Collision.h"
#include "../Components/SoldierInput.h"
#include "../Components/SoldierPhysics.h"

class Replay;

class Soldier : public Entity
{
public:
	Soldier();

	void update(Time dt, Replay *replay);
	void draw(SpriteBatch *batch, float framePercent);
	void reset(fixvec2 pos);
	void map(const Collision::Map *map);

	Sprite sprite;
	SoldierInput input;
	SoldierPhysics physics;
};
