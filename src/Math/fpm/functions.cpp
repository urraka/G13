#include <fix16.h>

#include "fixed.h"
#include "vec2.h"
#include "rect.h"
#include "line.h"

namespace fpm
{
	fixed fabs      (fixed const & x)                    { return fixed::from_value(fix16_abs  (x.value_));           }
	fixed ceil      (fixed const & x)                    { return fixed::from_value(fix16_ceil (x.value_));           }
	fixed floor     (fixed const & x)                    { return fixed::from_value(fix16_floor(x.value_));           }
	fixed sqrt      (fixed const & x)                    { return fixed::from_value(fix16_sqrt (x.value_));           }
	fixed exp       (fixed const & x)                    { return fixed::from_value(fix16_exp  (x.value_));           }
	fixed log       (fixed const & x)                    { return fixed::from_value(fix16_log  (x.value_));           }
	fixed sin       (fixed const & x)                    { return fixed::from_value(fix16_sin  (x.value_));           }
	fixed cos       (fixed const & x)                    { return fixed::from_value(fix16_cos  (x.value_));           }
	fixed tan       (fixed const & x)                    { return fixed::from_value(fix16_tan  (x.value_));           }
	fixed asin      (fixed const & x)                    { return fixed::from_value(fix16_asin (x.value_));           }
	fixed acos      (fixed const & x)                    { return fixed::from_value(fix16_acos (x.value_));           }
	fixed atan      (fixed const & x)                    { return fixed::from_value(fix16_atan (x.value_));           }
	fixed atan2     (fixed const & y, fixed const & x)   { return fixed::from_value(fix16_atan2(y.value_, x.value_)); }
	fixed min       (fixed const & x, fixed const & y)   { return fixed::from_value(fix16_min  (x.value_, y.value_)); }
	fixed max       (fixed const & x, fixed const & y)   { return fixed::from_value(fix16_max  (x.value_, y.value_)); }
	fixed fmod      (fixed const & x, fixed const & y)   { return fixed::from_value(fix16_mod  (x.value_, y.value_)); }

	fixed dot       (vec2 const & a, vec2 const & b)     { return a.x * b.x + a.y + b.y; }
	fixed length    (vec2 const & x)                     { return sqrt(x.x * x.x + x.y * x.y); }
	vec2  normalize (vec2 const & x)                     { fixed L = length(x); return vec2(x.x / L, x.y / L); }

	bool  contains  (rect const & rc, vec2 const & p)    { return p.x >= rc.tl.x && p.x <= rc.br.x && p.y >= rc.tl.y && p.y <= rc.br.y; }
	bool  intersects(rect const & rc1, rect const & rc2) { return rc1.tl.x < rc2.br.x && rc1.br.x > rc2.tl.x && rc1.tl.y < rc2.br.y && rc1.br.y > rc2.tl.y; }
	rect  expand    (rect const & rc1, rect const & rc2) { return rect(min(rc1.tl.x, rc2.tl.x), min(rc1.tl.y, rc2.tl.y), max(rc1.br.x, rc2.br.x), max(rc1.br.y, rc2.br.y)); }

	fixed slope       (line const & l) { return (l.p1.y - l.p2.y) / (l.p1.x - l.p2.x); }
	vec2  normal      (line const & l) { return normalize(vec2(-(l.p2.y - l.p1.y), l.p2.x - l.p1.x)); }
	vec2  midpoint    (line const & l) { return (l.p1 + l.p2) / fixed(2); }
	rect  bounds      (line const & l) { return rect(min(l.p1.x, l.p2.x), min(l.p1.y, l.p2.y), max(l.p1.x, l.p2.x), max(l.p1.y, l.p2.y)); }

	bool intersection(line const & l1, line const & l2, vec2 *result)
	{
		fixed x1 = l1.p1.x;
		fixed x2 = l1.p2.x;
		fixed x3 = l2.p1.x;
		fixed x4 = l2.p2.x;

		fixed y1 = l1.p1.y;
		fixed y2 = l1.p2.y;
		fixed y3 = l2.p1.y;
		fixed y4 = l2.p2.y;

		fixed uaNum = (x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3);
		fixed ubNum = (x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3);
		fixed uaDem = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);

		if (uaDem == fixed::zero)
			return false;

		fixed ua = uaNum / uaDem;
		fixed ub = ubNum / uaDem;

		if (ua < fixed::zero || ua > fixed::one || ub < fixed::zero || ub > fixed::one)
			return false;

		result->x = x1 + ua * (x2 - x1);
		result->y = y1 + ua * (y2 - y1);

		return true;
	}
}
