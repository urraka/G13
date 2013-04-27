#pragma once

#include "../Collision.h"
#include "../Components/SoldierInput.h"
#include "../Components/SoldierPhysics.h"

class Soldier : public Entity
{
public:
	Soldier();

	void update(Time dt);
	void draw(SpriteBatch *batch, float framePercent);
	void spawn(vec2 pos);
	void map(const Collision::Map *map);

private:
	Sprite sprite_;
	SoldierInput input_;
	SoldierPhysics physics_;
};
