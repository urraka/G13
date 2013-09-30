#include <fix16.h>

#include "fixed.h"
#include "vec2.h"
#include "rect.h"
#include "line.h"
#include "functions.h"

#include <assert.h>

namespace math {
namespace fpm {

// fixed functions

fixed sign(const fixed &x)
{
	return x > 0 ? 1 : x < 0 ? -1 : 0;
}

fixed fabs(const fixed &x)
{
	return fixed::from_value(fix16_abs(x.value_));
}

fixed ceil(const fixed &x)
{
	return fixed::from_value(fix16_ceil(x.value_));
}

fixed floor(const fixed &x)
{
	return fixed::from_value(fix16_floor(x.value_));
}

fixed sqrt(const fixed &x)
{
	return fixed::from_value(fix16_sqrt(x.value_));
}

fixed exp(const fixed &x)
{
	return fixed::from_value(fix16_exp(x.value_));
}

fixed log(const fixed &x)
{
	return fixed::from_value(fix16_log(x.value_));
}

fixed sin(const fixed &x)
{
	return fixed::from_value(fix16_sin(x.value_));
}

fixed cos(const fixed &x)
{
	return fixed::from_value(fix16_cos(x.value_));
}

fixed tan(const fixed &x)
{
	return fixed::from_value(fix16_tan(x.value_));
}

fixed asin(const fixed &x)
{
	return fixed::from_value(fix16_asin(x.value_));
}

fixed acos(const fixed &x)
{
	return fixed::from_value(fix16_acos(x.value_));
}

fixed atan(const fixed &x)
{
	return fixed::from_value(fix16_atan(x.value_));
}

fixed atan2(const fixed &y, const fixed &x)
{
	return fixed::from_value(fix16_atan2(y.value_, x.value_));
}

fixed min(const fixed &x, const fixed &y)
{
	return fixed::from_value(fix16_min(x.value_, y.value_));
}

fixed max(const fixed &x, const fixed &y)
{
	return fixed::from_value(fix16_max(x.value_, y.value_));
}

fixed fmod(const fixed &x, const fixed &y)
{
	return fixed::from_value(fix16_mod(x.value_, y.value_));
}

fixed epsilon_check(const fixed &x, const fixed &epsilon)
{
	return fabs(x) < epsilon ? fixed::zero : x;
}

// vec2 functions

fixed dot(const vec2 &a, const vec2 &b)
{
	return a.x * b.x + a.y * b.y;
}

fixed length(const vec2 &x)
{
	return x.x == 0 ? fabs(x.y) : x.y == 0 ? fabs(x.x) : sqrt(x.x * x.x + x.y * x.y);
}

fixed length2(const vec2 &x)
{
	return x.x * x.x + x.y * x.y;
}

vec2 normalize(const vec2 &x)
{
	fixed scalar;

	if (x.x != 0 && x.y != 0)
		scalar = max(fabs(x.x), fabs(x.y));
	else if (x.x == 0)
		scalar = fabs(x.y);
	else
		scalar = fabs(x.x);

	vec2 r = x / scalar;
	assert(r.x != fixed::overflow && r.y != fixed::overflow);

	fixed L = length(r);
	assert(L != fixed::overflow);

	r /= L;

	assert(r.x != fixed::overflow && r.y != fixed::overflow);

	return r;
}

vec2 sign(const vec2 &x)
{
	return vec2(sign(x.x), sign(x.y));
}

vec2 epsilon_check(const vec2 &x, const fixed &epsilon)
{
	return vec2(
		epsilon_check(x.x, epsilon),
		epsilon_check(x.y, epsilon)
	);
}

// rect functions

bool contains(const rect &rc, const vec2 &p)
{
	return p.x >= rc.tl.x && p.x <= rc.br.x && p.y >= rc.tl.y && p.y <= rc.br.y;
}

rect expand(const rect &rc1, const rect &rc2)
{
	return rect(
		min(rc1.tl.x, rc2.tl.x),
		min(rc1.tl.y, rc2.tl.y),
		max(rc1.br.x, rc2.br.x),
		max(rc1.br.y, rc2.br.y)
	);
}

bool intersects(const rect &rc1, const rect &rc2)
{
	return rc1.tl.x < rc2.br.x && rc1.br.x > rc2.tl.x && rc1.tl.y < rc2.br.y && rc1.br.y > rc2.tl.y;
}

bool intersects(const rect &rc, const line &line)
{
	fixed xmin = line.p1.x;
	fixed xmax = line.p2.x;

	if (line.p1.x > line.p2.x)
	{
		xmin = line.p2.x;
		xmax = line.p1.x;
	}

	if (xmax > rc.br.x)
		xmax = rc.br.x;

	if (xmin < rc.tl.x)
		xmin = rc.tl.x;

	if (xmin > xmax)
		return false;

	fixed ymin = line.p1.y;
	fixed ymax = line.p2.y;

	fixed dx = line.p2.x - line.p1.x;

	if (dx != 0)
	{
		fixed a = (line.p2.y - line.p1.y) / dx;
		fixed b = line.p1.y - a * line.p1.x;

		ymin = a * xmin + b;
		ymax = a * xmax + b;
	}

	if (ymin > ymax)
	{
		fixed tmp = ymax;
		ymax = ymin;
		ymin = tmp;
	}

	if (ymax > rc.br.y)
		ymax = rc.br.y;

	if (ymin < rc.tl.y)
		ymin = rc.tl.y;

	if (ymin > ymax)
		return false;

	return true;
}

// line functions

fixed slope(const line &l)
{
	return (l.p1.y - l.p2.y) / (l.p1.x - l.p2.x);
}

vec2 normal(const line &l)
{
	return normalize(vec2(-(l.p2.y - l.p1.y), l.p2.x - l.p1.x));
}

vec2 midpoint(const line &l)
{
	return (l.p1 + l.p2) / fixed(2);
}

rect bounds(const line &l)
{
	return rect(
		min(l.p1.x, l.p2.x),
		min(l.p1.y, l.p2.y),
		max(l.p1.x, l.p2.x),
		max(l.p1.y, l.p2.y)
	);
}

bool intersection(const line &A, const line &B, vec2 *result)
{
	int64_t x1 = A.p1.x.value();
	int64_t x2 = A.p2.x.value();
	int64_t x3 = B.p1.x.value();
	int64_t x4 = B.p2.x.value();

	int64_t y1 = A.p1.y.value();
	int64_t y2 = A.p2.y.value();
	int64_t y3 = B.p1.y.value();
	int64_t y4 = B.p2.y.value();

	int64_t uaNum = (((x4 - x3) * (y1 - y3)) >> 16) - (((y4 - y3) * (x1 - x3)) >> 16);
	int64_t ubNum = (((x2 - x1) * (y1 - y3)) >> 16) - (((y2 - y1) * (x1 - x3)) >> 16);
	int64_t uaDem = (((y4 - y3) * (x2 - x1)) >> 16) - (((x4 - x3) * (y2 - y1)) >> 16);

	if (uaDem == 0)
		return false;

	int64_t ua = (uaNum << 16) / uaDem;
	int64_t ub = (ubNum << 16) / uaDem;

	if (ua < 0 || ua > fixed::one.value() || ub < 0 || ub > fixed::one.value())
		return false;

	result->x = A.p1.x + fixed::from_value(ua) * (A.p2.x - A.p1.x);
	result->y = A.p1.y + fixed::from_value(ua) * (A.p2.y - A.p1.y);

	return true;
}

}} // math::fpm
