#pragma once

#include <glm/glm.hpp>
#include <stdint.h>
#include <vector>

namespace math {

std::vector<uint16_t> triangulate(const std::vector<glm::vec2> &polygon);

} // math
