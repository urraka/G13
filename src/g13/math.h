#pragma once

#include <glm/glm.hpp>
#include <math/interpolable.h>
#include <math/minbits.h>
#include <math/fpm/fpm.h>

namespace g13 {

namespace fpm = math::fpm;

typedef glm::mat4  mat4;
typedef glm::vec2  vec2;
typedef fpm::fixed fixed;
typedef fpm::vec2  fixvec2;
typedef fpm::rect  fixrect;
typedef fpm::line  fixline;

static inline vec2 from_fixed(fixvec2 x)
{
	return vec2(x.x.to_float(), x.y.to_float());
}

static inline fixvec2 to_fixed(vec2 x)
{
	return fixvec2(x.x, x.y);
}

} // g13