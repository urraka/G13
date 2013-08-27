#pragma once

#include <math.h>

namespace math {

inline float wrap_angle(float angle, float min)
{
	const float max = min + 2 * M_PI;

	while (angle <  min) angle += 2 * M_PI;
	while (angle >= max) angle -= 2 * M_PI;

	return angle;
}

} // math
