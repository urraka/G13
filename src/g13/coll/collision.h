#pragma once

#include <g13/math.h>
#include <vector>

#include "Segment.h"
#include "Grid.h"
#include "Hull.h"
#include "Entity.h"
#include "Result.h"
#include "World.h"

namespace g13 {
namespace coll {

inline bool is_floor(const fixline &line)
{
	return line.p1.x != line.p2.x && fpm::fabs(fpm::slope(line)) <= 2 &&
		fpm::dot(fixvec2(0, -1), fpm::normal(line)) > 0;
}

inline bool is_floor(const fixvec2 &a, const fixvec2 &b)
{
	return is_floor(fixline(a, b));
}

}}
