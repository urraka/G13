#include <Math/math.h>

Triangle::Triangle() {}

Triangle::Triangle(vec2 v1, vec2 v2, vec2 v3)
	:	a(v1),
		b(v2),
		c(v3)
{
}

bool Triangle::contains(const vec2 &point)
{
	float A = 0.5f * (-b.y * c.x + a.y * (-b.x + c.x) + a.x * (b.y - c.y) + b.x * c.y);
	float sign = A < 0.0f ? -1.0f : 1.0f;

	float s = (a.y * c.x - a.x * c.y + (c.y - a.y) * point.x + (a.x - c.x) * point.y) * sign;
	float t = (a.x * b.y - a.y * b.x + (a.y - b.y) * point.x + (b.x - a.x) * point.y) * sign;

	return s >= 0.0f && t >= 0.0f && (s + t) <= (2.0f * A * sign);
}
