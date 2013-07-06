#pragma once

#include "../Collision.h"

namespace cmp {

class SoldierInput;

class SoldierPhysics
{
public:
	SoldierPhysics();

	void update(Time time);
	void reset(fixvec2 pos);
	bool ducking() const;
	bool floor() const;

	fixrect bboxNormal;
	fixrect bboxDucked;
	fixvec2 position;
	fixvec2 velocity;
	fixvec2 acceleration;

	const SoldierInput *input;
	const Collision::Map *map;

private:
	bool ducked_;

	Collision::Hull currentHull_;
	const Collision::Node *currentNode_;
};

} // cmp
