#include <fix16.h>
#include <ostream>

#include "fixed.h"
#include "vec2.h"
#include "rect.h"
#include "line.h"
#include "functions.h"
#include "constants.h"

#include <assert.h>

namespace math {
namespace fpm {

// conversion

fixed from_value(int32_t value)
{
	fixed x;
	x.value_ = value;
	return x;
}

fixed from_string(const char *str)
{
	return from_value(fix16_from_str(str));
}

const char *to_string(const fixed &x, int precision)
{
	static char buffer[16];
	fix16_to_str(x.value_, buffer, precision);
	return buffer;
}

int to_int(const fixed &x)
{
	return fix16_to_int(x.value_);
}

float to_float(const fixed &x)
{
	return fix16_to_float(x.value_);
}

double to_double(const fixed &x)
{
	return fix16_to_dbl(x.value_);
}

// fixed functions

fixed sign(const fixed &x)
{
	return x > 0 ? 1 : x < 0 ? -1 : 0;
}

fixed fabs(const fixed &x)
{
	return from_value(fix16_abs(x.value_));
}

fixed ceil(const fixed &x)
{
	return from_value(fix16_ceil(x.value_));
}

fixed floor(const fixed &x)
{
	return from_value(fix16_floor(x.value_));
}

fixed sqrt(const fixed &x)
{
	return from_value(fix16_sqrt(x.value_));
}

fixed exp(const fixed &x)
{
	return from_value(fix16_exp(x.value_));
}

fixed log(const fixed &x)
{
	return from_value(fix16_log(x.value_));
}

fixed sin(const fixed &x)
{
	return from_value(fix16_sin(x.value_));
}

fixed cos(const fixed &x)
{
	return from_value(fix16_cos(x.value_));
}

fixed tan(const fixed &x)
{
	return from_value(fix16_tan(x.value_));
}

fixed asin(const fixed &x)
{
	return from_value(fix16_asin(x.value_));
}

fixed acos(const fixed &x)
{
	return from_value(fix16_acos(x.value_));
}

fixed atan(const fixed &x)
{
	return from_value(fix16_atan(x.value_));
}

fixed atan2(const fixed &y, const fixed &x)
{
	return from_value(fix16_atan2(y.value_, x.value_));
}

fixed min(const fixed &x, const fixed &y)
{
	return from_value(fix16_min(x.value_, y.value_));
}

fixed max(const fixed &x, const fixed &y)
{
	return from_value(fix16_max(x.value_, y.value_));
}

fixed clamp(const fixed &x, const fixed &a, const fixed &b)
{
	return min(b, max(a, x));
}

fixed fmod(const fixed &x, const fixed &y)
{
	return from_value(fix16_mod(x.value_, y.value_));
}

fixed epsilon_check(const fixed &x, const fixed &epsilon)
{
	return fabs(x) < epsilon ? Zero : x;
}

fixed lerp(const fixed &a, const fixed &b, const fixed &step)
{
	return a + (b - a) * step;
}

fixed radians(const fixed &x)
{
	static const fixed to_rad = from_value(1144);

	return x * to_rad;
}

fixed degrees(const fixed &x)
{
	static const fixed to_deg = from_value(3754936);

	return x * to_deg;
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
	assert(r.x != Overflow && r.y != Overflow);

	fixed L = length(r);
	assert(L != Overflow);

	r /= L;

	assert(r.x != Overflow && r.y != Overflow);

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

vec2 lerp(const vec2 &a, const vec2 &b, const fixed &step)
{
	return vec2(lerp(a.x, b.x, step), lerp(a.y, b.y, step));
}

// rect functions

bool contains(const rect &rc, const vec2 &p)
{
	return p.x >= rc.tl.x && p.x <= rc.br.x && p.y >= rc.tl.y && p.y <= rc.br.y;
}

bool contains(const rect &rc1, const rect &rc2)
{
	return contains(rc1, rc2.tl) && contains(rc1, rc2.br);
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
	const fpm::rect RC = rc - line.p1;
	const fpm::line L(vec2(0, 0), line.p2 - line.p1);

	fixed xmin = L.p1.x;
	fixed xmax = L.p2.x;

	if (L.p1.x > L.p2.x)
	{
		xmin = L.p2.x;
		xmax = L.p1.x;
	}

	if (xmax > RC.br.x)
		xmax = RC.br.x;

	if (xmin < RC.tl.x)
		xmin = RC.tl.x;

	if (xmin > xmax)
		return false;

	fixed ymin = L.p1.y;
	fixed ymax = L.p2.y;

	fixed dx = L.p2.x - L.p1.x;

	if (dx != 0)
	{
		fixed a = (L.p2.y - L.p1.y) / dx;
		fixed b = L.p1.y - a * L.p1.x;

		ymin = a * xmin + b;
		ymax = a * xmax + b;
	}

	if (ymin > ymax)
	{
		fixed tmp = ymax;
		ymax = ymin;
		ymin = tmp;
	}

	if (ymax > RC.br.y)
		ymax = RC.br.y;

	if (ymin < RC.tl.y)
		ymin = RC.tl.y;

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
	int64_t x1 = A.p1.x.value_;
	int64_t x2 = A.p2.x.value_;
	int64_t x3 = B.p1.x.value_;
	int64_t x4 = B.p2.x.value_;

	int64_t y1 = A.p1.y.value_;
	int64_t y2 = A.p2.y.value_;
	int64_t y3 = B.p1.y.value_;
	int64_t y4 = B.p2.y.value_;

	int64_t uaNum = (((x4 - x3) * (y1 - y3)) >> 16) - (((y4 - y3) * (x1 - x3)) >> 16);
	int64_t ubNum = (((x2 - x1) * (y1 - y3)) >> 16) - (((y2 - y1) * (x1 - x3)) >> 16);
	int64_t uaDem = (((y4 - y3) * (x2 - x1)) >> 16) - (((x4 - x3) * (y2 - y1)) >> 16);

	if (uaDem == 0)
		return false;

	int64_t ua = (uaNum << 16) / uaDem;
	int64_t ub = (ubNum << 16) / uaDem;

	if (ua < 0 || ua > One.value_ || ub < 0 || ub > One.value_)
		return false;

	result->x = A.p1.x + from_value(ua) * (A.p2.x - A.p1.x);
	result->y = A.p1.y + from_value(ua) * (A.p2.y - A.p1.y);

	return true;
}

}} // math::fpm

std::ostream& operator<<(std::ostream &stream, const math::fpm::fixed &x)
{
	char buf[13];
	fix16_to_str(x.value_, buf, std::min(5, (int)stream.precision()));
	stream << buf;
	return stream;
}

std::ostream& operator<<(std::ostream &stream, const math::fpm::vec2 &x)
{
	return stream << "(" << x.x << "," << x.y << ")";
}
