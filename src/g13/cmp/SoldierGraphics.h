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
	enum SpriteIndex
	{
		Leg1,
		Leg2,
		Body,
		Head,
		Eye1,
		Eye2,
		ArmBack,
		Weapon,
		ArmFront,
		SpriteCount
	};

public:
	SoldierGraphics();

	void update(Time dt, const SoldierState &state);
	void frame(const Frame &frame);

	const gfx::Sprite (&sprites())[SpriteCount];

	void aim(uint16_t &angle, bool &rightwards) const
	{
		angle = angle_;
		rightwards = rightwards_;
	}

	gfx::Sprite sprite;
	math::interpolable<vec2> position;
	vec2 *target;

private:
	math::interpolable<float> time_;
	gfx::Sprite sprites_[SpriteCount];

	bool running_;
	bool air_;

	float runningTime_;
	float speed_;

	math::interpolable<uint16_t> angle_;
	bool rightwards_;
};

}} // g13::cmp
