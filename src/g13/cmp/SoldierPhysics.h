#pragma once

#include <g13/g13.h>
#include <g13/math.h>
#include <g13/coll/collision.h>

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
	const coll::World *world;

private:
	bool ducked_;

	coll::Hull hull_;
	const coll::Segment *segment_;
};

}} // g13::cmp
