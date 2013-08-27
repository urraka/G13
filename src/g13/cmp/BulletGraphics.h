#pragma once

#include <g13/g13.h>
#include <g13/math.h>
#include <gfx/forward.h>
#include <math/mix_angle.h>

namespace g13 {
namespace cmp {

class BulletPhysics;

class BulletGraphics
{
public:
	void update(Time dt, const BulletPhysics *physics);
	void frame(Frame frame);
	gfx::Sprite sprite() const;

	math::interpolable<vec2> position;
	math::interpolable<float, math::mix_angle> angle;
};

}} // g13::cmp
