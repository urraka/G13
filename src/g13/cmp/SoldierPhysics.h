#pragma once

#include <g13/g13.h>
#include <g13/math.h>
#include <g13/Collision.h>

namespace g13 {
namespace cmp {

class SoldierPhysics
{
public:
	SoldierPhysics();

	void update(Time dt);
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

}} // g13::cmp
