#pragma once

#include "fixed.h"
#include "vec2.h"
#include "rect.h"
#include "line.h"

namespace fpm
{
	// fixed
	fixed sign (fixed const & x);
	fixed fabs (fixed const & x);
	fixed ceil (fixed const & x);
	fixed floor(fixed const & x);
	fixed sqrt (fixed const & x);
	fixed exp  (fixed const & x);
	fixed log  (fixed const & x);
	fixed sin  (fixed const & x);
	fixed cos  (fixed const & x);
	fixed tan  (fixed const & x);
	fixed asin (fixed const & x);
	fixed acos (fixed const & x);
	fixed atan (fixed const & x);
	fixed atan2(fixed const & y, fixed const & x);
	fixed min  (fixed const & x, fixed const & y);
	fixed max  (fixed const & x, fixed const & y);
	fixed fmod (fixed const & x, fixed const & y);

	// vec2
	fixed dot       (vec2 const & a, vec2 const & b);
	fixed length    (vec2 const & x);
	fixed length2   (vec2 const & x);
	vec2  normalize (vec2 const & x);
	vec2  sign      (vec2 const & x);

	// rect
	bool  contains  (rect const & rc, vec2 const & p);
	bool  intersects(rect const & rc1, rect const & rc2);
	rect  expand    (rect const & rc1, rect const & rc2);

	// line
	fixed slope       (line const & l);
	vec2  normal      (line const & l);
	vec2  midpoint    (line const & l);
	rect  bounds      (line const & l);
	bool  intersection(line const & l1, line const & l2, vec2 *result);
}
