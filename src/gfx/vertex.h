#pragma once

// Each module should define its own version of VERTEX_INSTANCE before including vertex.h.
// It's used for template instantiations (if necessary).

#ifndef VERTEX_INSTANCE
	#define VERTEX_INSTANCE(T)
#endif

#define VERTEX_INSTANCES()         \
	VERTEX_INSTANCE(ColorVertex ); \
	VERTEX_INSTANCE(SpriteVertex);

#include <stdint.h>

namespace gfx {

struct ColorVertex
{
	float x, y;
	uint8_t r, g, b, a;
};

struct SpriteVertex
{
	float x, y;
	float u, v;
	uint8_t opacity;
};

ColorVertex color_vertex(float x, float y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
SpriteVertex sprite_vertex(float x, float y, float u, float v, uint8_t opacity);

} // gfx
