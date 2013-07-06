#pragma once

#include <stdint.h>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_precision.hpp>
#include <glm/gtc/epsilon.hpp>

using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::ivec2;
using glm::u8vec4;

#include "fpm/fpm.h"
#include "interpolable.h"
#include "minbits.h"
#include "Triangle.h"

typedef fpm::fixed fixed;
typedef fpm::vec2 fixvec2;
typedef fpm::rect fixrect;
typedef fpm::line fixline;

namespace math {

std::vector<uint16_t> triangulate(const std::vector<vec2> &polygon);

static inline vec2 from_fixed(fixvec2 x)
{
	return vec2(x.x.to_float(), x.y.to_float());
}

static inline fixvec2 to_fixed(vec2 x)
{
	return fixvec2(x.x, x.y);
}

} // math
