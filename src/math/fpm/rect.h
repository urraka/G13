#pragma once

#include "fixed.h"
#include "functions.h"
#include "constants.h"
#include "vec2.h"

namespace math {
namespace fpm {

class rect
{
public:
	vec2 tl;
	vec2 br;

	rect() {}
	rect(vec2 tl, vec2 br) : tl(tl), br(br) {}
	rect(const fixed &l, const fixed &t, const fixed &r, const fixed &b) : tl(l, t), br(r, b) {}

	vec2  size  () const { return br - tl; }
	fixed width () const { return br.x - tl.x; }
	fixed height() const { return br.y - tl.y; }
	vec2  center() const { return lerp(tl, br, Half); }

	rect operator+(const vec2 &rhs) const { return rect(tl + rhs, br + rhs); }
	rect operator-(const vec2 &rhs) const { return rect(tl - rhs, br - rhs); }

	rect& operator+=(const vec2 &rhs) { tl += rhs; br += rhs; return *this; }
	rect& operator-=(const vec2 &rhs) { tl -= rhs; br -= rhs; return *this; }
};

}} // math::fpm

inline math::fpm::rect operator+(const math::fpm::vec2 &v, const math::fpm::rect &rc) { return rc + v; }
