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
		scale(1.0f)
{
}

void Sprite::vertices(SpriteVertex (&vertex)[4]) const
{
	vertex[0] = sprite_vertex( 0.0f,   0.0f, tx0.x, tx0.y, color);
	vertex[1] = sprite_vertex(width,   0.0f, tx1.x, tx0.y, color);
	vertex[2] = sprite_vertex(width, height, tx1.x, tx1.y, color);
	vertex[3] = sprite_vertex( 0.0f, height, tx0.x, tx1.y, color);

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
