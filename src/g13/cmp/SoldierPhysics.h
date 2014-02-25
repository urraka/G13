#pragma once

#include <g13/g13.h>
#include <g13/math.h>
#include <g13/coll/Hull.h>

namespace g13 {
namespace cmp {

class SoldierPhysics
{
public:
	SoldierPhysics();

	enum Mode { Normal, Rope };

	void update(Time dt, const coll::World &world, const SoldierInput &input);
	void hook(const fixvec2 &hookPosition);
	void unhook();
	void reset(fixvec2 pos);

	bool ducking() const { return ducked; }
	bool floor() const { return segment != 0 && segment->floor; }
	const fixrect &bounds() const { return ducked ? bboxDucked : bboxNormal; }

	const fixrect bboxNormal;
	const fixrect bboxDucked;

	Mode    mode;
	fixvec2 position;
	fixvec2 velocity;
	fixvec2 acceleration;
	fixed   walkvel;

private:
	bool ducked;
	coll::Hull hull;
	const coll::Segment *segment;
	fixvec2 ropeHook;

	void updateNormal(Time dt, const coll::World &world, const SoldierInput &input);
	void updateRope(Time dt, const coll::World &world, const SoldierInput &input);

	#ifdef DBG_SHOW_CURRENT_HULL
	friend class ::g13::net::Renderer;
	#endif
};

}} // g13::cmp
