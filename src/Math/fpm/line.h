#pragma once

#include "fixed.h"
#include "vec2.h"

namespace fpm {

class line
{
public:
	vec2 p1;
	vec2 p2;

	line()                                                                {}
	line(vec2 P1, vec2 P2)                       : p1(P1),     p2(P2)     {}
	line(fixed x1, fixed y1, fixed x2, fixed y2) : p1(x1, y1), p2(x2, y2) {}
};

} // fpm
