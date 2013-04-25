#pragma once

#include "../Collision.h"
#include "SoldierInput.h"

class SoldierPhysics
{
public:
	SoldierPhysics();

	void update(Time time);
	void teleport(fixvec2 pos);

	fixrect bbox;
	fixvec2 position;
	fixvec2 velocity;
	fixvec2 acceleration;
	SoldierInput input;
	const Collision::Map *map;

private:
	bool ducking_;

	Collision::Hull currentHull_;
	const Collision::Node *currentNode_;
};
