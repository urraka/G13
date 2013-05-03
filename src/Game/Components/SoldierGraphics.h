#pragma once

#include "../../Math/math.h"
#include "../../Graphics/Sprite.h"
#include "../../System/Clock.h"
#include "../Animation.h"

class SoldierInput;
class SoldierPhysics;

class SoldierGraphics
{
public:
	SoldierGraphics();
	void update(Time dt);
	void frame(float percent);

	Sprite sprite;
	Animation animation;
	math::interpolable<vec2> position;

	const SoldierInput *input;
	const SoldierPhysics *physics;

private:
	void updateSprite(const Frame *frame);

	enum Animations
	{
		Standing = 0,
		Ducking,
		Walking,
		DuckWalking,
		Falling,
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
