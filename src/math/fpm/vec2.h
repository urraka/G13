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

	vec2(fixed x, fixed y) : x(x), y(y) {}
	vec2(int x, int y)     : x(x), y(y) {}

	vec2(fixed x) : x(x), y(x) {}
	vec2(int x)   : x(x), y(x) {}

	vec2 operator- ()                  const { return vec2(-x,        -y);         }
	vec2 operator+ (vec2  const & rhs) const { return vec2( x + rhs.x, y + rhs.y); }
	vec2 operator- (vec2  const & rhs) const { return vec2( x - rhs.x, y - rhs.y); }
	vec2 operator* (fixed const & rhs) const { return vec2( x * rhs,   y * rhs);   }
	vec2 operator/ (fixed const & rhs) const { return vec2( x / rhs,   y / rhs);   }

	vec2& operator+=(vec2  const & rhs) { x += rhs.x; y += rhs.y; return *this; }
	vec2& operator-=(vec2  const & rhs) { x -= rhs.x; y -= rhs.y; return *this; }
	vec2& operator*=(fixed const & rhs) { x *= rhs;   y *= rhs;   return *this; }
	vec2& operator/=(fixed const & rhs) { x /= rhs;   y /= rhs;   return *this; }

	bool operator==(vec2 const & rhs) const { return x == rhs.x && y == rhs.y; }
	bool operator!=(vec2 const & rhs) const { return x != rhs.x || y != rhs.y; }
};

}} // math::fpm
