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
	vertex[0] = sprite_vertex(-center.x        , -center.y         , tx0.x, tx0.y, color);
	vertex[1] = sprite_vertex(-center.x + width, -center.y         , tx1.x, tx0.y, color);
	vertex[2] = sprite_vertex(-center.x + width, -center.y + height, tx1.x, tx1.y, color);
	vertex[3] = sprite_vertex(-center.x        , -center.y + height, tx0.x, tx1.y, color);

	mat2d m = transform;

	m *= mat2d::translate(position.x, position.y);
	m *= mat2d::rotate(rotation);
	m *= mat2d::scale(scale.x, scale.y);

	for (size_t i = 0; i < 4; i++)
		vertex[i] = m * vertex[i];
}

} // gfx
