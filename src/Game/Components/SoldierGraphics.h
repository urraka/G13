#pragma once

#include "../../Math/math.h"
#include "../../Graphics/Graphics.h"
#include "../../System/Clock.h"

class SoldierInput;
class SoldierPhysics;

class SoldierGraphics
{
public:
	SoldierGraphics();
	void update(Time dt);
	void frame(float percent);

	Sprite sprite;
	math::interpolable<vec2> position;

	const SoldierInput *input;
	const SoldierPhysics *physics;
};
