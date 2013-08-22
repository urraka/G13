#pragma once

#include <glm/glm.hpp>

namespace math
{
	inline float mix_angle(float a, float b, float step)
	{
		while (a < 0.0f)      a += 2 * M_PI;
		while (a >= 2 * M_PI) a -= 2 * M_PI;
		while (b < 0.0f)      b += 2 * M_PI;
		while (b >= 2 * M_PI) b -= 2 * M_PI;

		if (glm::abs(a - b) < M_PI)
			return glm::mix(a, b, step);

		if (a < b)
			a += 2 * M_PI;
		else
			b += 2 * M_PI;

		float result = glm::mix(a, b, step);

		if (result >= 2 * M_PI)
			result -= 2 * M_PI;

		return result;
	}
}
