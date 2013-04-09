#pragma once

#include <Math/math.h>

class Triangle
{
public:
	Triangle();
	Triangle(vec2 v1, vec2 v2, vec2 v3);
	bool contains(const vec2 &point);

	vec2 a;
	vec2 b;
	vec2 c;
};
