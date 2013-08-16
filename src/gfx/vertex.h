#pragma once

// Each module should define its own version of VERTEX_INSTANCE before including vertex.h.
// It's used for template instantiations (if necessary).

#ifndef VERTEX_INSTANCE
	#define VERTEX_INSTANCE(T)
#endif

#define VERTEX_INSTANCES()         \
	VERTEX_INSTANCE(ColorVertex ); \
	VERTEX_INSTANCE(SpriteVertex); \
	VERTEX_INSTANCE(TextVertex);

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
	uint8_t r, g, b, a;
};

struct TextVertex
{
	float x, y;
	uint16_t u, v;
};

} // gfx
