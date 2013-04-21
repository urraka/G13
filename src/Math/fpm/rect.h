#pragma once

#include "fixed.h"
#include "vec2.h"

namespace fpm
{
	class rect
	{
	public:
		vec2 tl;
		vec2 br;

		rect()                                   : tl()    , br()     {}
		rect(vec2 TL, vec2 BR)                   : tl(TL)  , br(BR)   {}
		rect(fixed l, fixed t, fixed r, fixed b) : tl(l, t), br(r, b) {}

		vec2  size  () const { return br - tl; }
		fixed width () const { return br.x - tl.x; }
		fixed height() const { return br.y - tl.y; }

		rect operator+ (vec2 const & rhs) const { return rect(tl + rhs, br + rhs); }
		rect operator- (vec2 const & rhs) const { return rect(tl - rhs, br - rhs); }

		rect& operator+=(vec2 const & rhs) { tl += rhs; br += rhs; return *this; }
		rect& operator-=(vec2 const & rhs) { tl -= rhs; br -= rhs; return *this; }
	};
}
