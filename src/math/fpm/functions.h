#pragma once

namespace math {
namespace fpm {

class fixed;
class vec2;
class rect;
class line;

// fixed
fixed sign         (const fixed &x);
fixed fabs         (const fixed &x);
fixed ceil         (const fixed &x);
fixed floor        (const fixed &x);
fixed sqrt         (const fixed &x);
fixed exp          (const fixed &x);
fixed log          (const fixed &x);
fixed sin          (const fixed &x);
fixed cos          (const fixed &x);
fixed tan          (const fixed &x);
fixed asin         (const fixed &x);
fixed acos         (const fixed &x);
fixed atan         (const fixed &x);
fixed atan2        (const fixed &y, const fixed &x);
fixed min          (const fixed &x, const fixed &y);
fixed max          (const fixed &x, const fixed &y);
fixed fmod         (const fixed &x, const fixed &y);
fixed epsilon_check(const fixed &x, const fixed &epsilon);
fixed lerp         (const fixed &a, const fixed &b, const fixed &step);
fixed radians      (const fixed &x);
fixed degrees      (const fixed &x);

// vec2
fixed dot          (const vec2 &a, const vec2 &b);
fixed length       (const vec2 &x);
fixed length2      (const vec2 &x);
vec2  normalize    (const vec2 &x);
vec2  sign         (const vec2 &x);
vec2  epsilon_check(const vec2 &x, const fixed &epsilon);
vec2  lerp         (const vec2 &a, const vec2 &b, const fixed &step);

// rect
bool  contains  (const rect &rc, const vec2 &p);
bool  contains  (const rect &rc1, const rect &rc2);
rect  expand    (const rect &rc1, const rect &rc2);
bool  intersects(const rect &rc1, const rect &rc2);
bool  intersects(const rect &rc, const line &line);

// line
fixed slope       (const line &l);
vec2  normal      (const line &l);
vec2  midpoint    (const line &l);
rect  bounds      (const line &l);
bool  intersection(const line &A, const line &B, vec2 *result);

}} // math::fpm
