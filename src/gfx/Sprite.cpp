#include "Sprite.h"
#include "vertex.h"
#include "gfx.h"
#include <stdint.h>

namespace gfx {

Sprite::Sprite()
	:	color(255, 255, 255, 255),
		texture(0),
		width(0.0f),
		height(0.0f),
		rotation(0.0f),
		tx1(1.0f),
		scale(1.0f),
		textureRotated(false)
{
}

void Sprite::vertices(SpriteVertex (&vertex)[4]) const
{
	float u0 = tx0.x, v0 = tx0.y;
	float u1 = tx1.x, v1 = tx0.y;
	float u2 = tx1.x, v2 = tx1.y;
	float u3 = tx0.x, v3 = tx1.y;

	if (textureRotated && (tx1.x - tx0.x < 0 || tx1.y - tx0.y < 0))
	{
		u0 = tx0.x, v0 = tx0.y;
		u1 = tx0.x, v1 = tx1.y;
		u2 = tx1.x, v2 = tx1.y;
		u3 = tx1.x, v3 = tx0.y;
	}

	vertex[0] = sprite_vertex( 0.0f,   0.0f, u0, v0, color);
	vertex[1] = sprite_vertex(width,   0.0f, u1, v1, color);
	vertex[2] = sprite_vertex(width, height, u2, v2, color);
	vertex[3] = sprite_vertex( 0.0f, height, u3, v3, color);

	const float &x  = position.x;
	const float &y  = position.y;
	const float &sx = scale.x;
	const float &sy = scale.y;
	const float &ox = center.x;
	const float &oy = center.y;

	mat2d m = transform * mat2d::transform(x, y, rotation, sx, sy, ox, oy, 0.0f, 0.0f);

	for (size_t i = 0; i < 4; i++)
		vertex[i] = m * vertex[i];
}

} // gfx
