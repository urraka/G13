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
#include "Triangle.h"

namespace math
{
	std::vector<uint16_t> triangulate(const std::vector<vec2> &polygon);
}
