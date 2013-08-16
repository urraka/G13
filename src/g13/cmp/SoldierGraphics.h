#pragma once

#include <g13/g13.h>
#include <g13/math.h>
#include <g13/Animation.h>

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
	void frame(float percent);
	const gfx::Sprite (&sprites())[SpriteCount];

	gfx::Sprite sprite;
	Animation animation;
	math::interpolable<vec2> position;
	vec2 target;

private:
	math::interpolable<float> time_;
	gfx::Sprite sprites_[SpriteCount];

	bool moving_;
	bool flip_;

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
