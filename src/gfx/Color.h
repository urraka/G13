#pragma once

#include <stdint.h>

namespace gfx {

struct Color
{
	Color() : r(0), g(0), b(0), a(0) {}
	Color(uint8_t C) : r(C), g(C), b(C), a(255) {}
	Color(uint8_t C, uint8_t A) : r(C), g(C), b(C), a(A) {}
	Color(uint8_t R, uint8_t G, uint8_t B, uint8_t A = 255) : r(R), g(G), b(B), a(A) {}

	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

} // gfx
