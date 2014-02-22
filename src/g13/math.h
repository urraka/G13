#pragma once

#include <glm/glm.hpp>
#include <gfx/mat2d.h>
#include <math/interpolable.h>
#include <math/minbits.h>
#include <math/fpm/fpm.h>
#include <hlp/countof.h>

namespace g13 {

namespace fpm = math::fpm;

typedef gfx::mat2d mat2d;
typedef glm::vec2  vec2;
typedef fpm::fixed fixed;
typedef fpm::vec2  fixvec2;
typedef fpm::rect  fixrect;
typedef fpm::line  fixline;

static inline vec2 from_fixed(const fixvec2 &x)
{
	return vec2(fpm::to_float(x.x), fpm::to_float(x.y));
}

static inline fixvec2 to_fixed(const vec2 &x)
{
	return fixvec2(x.x, x.y);
}

} // g13
