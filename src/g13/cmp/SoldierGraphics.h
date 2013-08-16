#pragma once

#include <g13/g13.h>
#include <g13/math.h>

#include <gfx/forward.h>
#include <gfx/Sprite.h>

namespace g13 {
namespace cmp {

class SoldierState;

class SoldierGraphics
{
	static const int SpriteCount = 9;

public:
	SoldierGraphics();

	void update(Time dt, const SoldierState &state);
	void frame(const Frame &frame);

	const gfx::Sprite (&sprites())[SpriteCount];

	gfx::Sprite sprite;
	math::interpolable<vec2> position;
	vec2 target;

private:
	math::interpolable<float> time_;
	gfx::Sprite sprites_[SpriteCount];

	bool moving_;
	bool flip_;
};

}} // g13::cmp
