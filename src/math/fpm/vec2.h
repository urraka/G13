#pragma once

#include "fixed.h"

namespace math {
namespace fpm {

class vec2
{
public:
	fixed x;
	fixed y;

	vec2() : x(0), y(0) {}

	vec2(const fixed &x, const fixed &y) : x(x), y(y) {}
	vec2(int x, int y)                   : x(x), y(y) {}

	explicit vec2(const fixed &x) : x(x), y(x) {}
	explicit vec2(int x)          : x(x), y(x) {}

	vec2 operator-()                 const { return vec2(-x,        -y);         }
	vec2 operator+(const vec2  &rhs) const { return vec2( x + rhs.x, y + rhs.y); }
	vec2 operator-(const vec2  &rhs) const { return vec2( x - rhs.x, y - rhs.y); }
	vec2 operator*(const fixed &rhs) const { return vec2( x * rhs,   y * rhs);   }
	vec2 operator/(const fixed &rhs) const { return vec2( x / rhs,   y / rhs);   }

	vec2& operator+=(const vec2  &rhs) { x += rhs.x; y += rhs.y; return *this; }
	vec2& operator-=(const vec2  &rhs) { x -= rhs.x; y -= rhs.y; return *this; }
	vec2& operator*=(const fixed &rhs) { x *= rhs;   y *= rhs;   return *this; }
	vec2& operator/=(const fixed &rhs) { x /= rhs;   y /= rhs;   return *this; }

	bool operator==(const vec2 &rhs) const { return x == rhs.x && y == rhs.y; }
	bool operator!=(const vec2 &rhs) const { return x != rhs.x || y != rhs.y; }
};

}} // math::fpm

inline math::fpm::vec2 operator*(const math::fpm::fixed &a, const math::fpm::vec2 &v) { return v * a; }
inline math::fpm::vec2 operator*(int a, const math::fpm::vec2 &v) { return v * math::fpm::fixed(a); }
