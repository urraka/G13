#pragma once

#include "SoldierState.h"

#include <g13/g13.h>
#include <g13/math.h>
#include <g13/Animation.h>

#include <gfx/Sprite.h>

namespace g13 {
namespace cmp {

class SoldierGraphics
{
public:
	SoldierGraphics();
	void update(Time dt, const SoldierState &state);
	void frame(float percent);

	gfx::Sprite sprite;
	Animation animation;
	math::interpolable<vec2> position;

private:
	void updateSprite(const Frame *frame);

	enum Animations
	{
		Standing = 0,
		Ducking,
		Walking,
		DuckWalking,
		Jumping,
		Falling,
		DuckJumping,
		DuckFalling,
		AnimationsCount
	};

	enum Frames
	{
		Standing00 = 0,
		Ducking00,
		Walking00,
		Walking01,
		DuckWalking00,
		DuckWalking01,
		Falling00,
		DuckFalling00,
		FramesCount
	};

	struct AnimationData
	{
		AnimationData();
		Frame frames[FramesCount];
		AnimationInfo animations[AnimationsCount];
	};

	static AnimationData data_;
};

}} // g13::cmp
