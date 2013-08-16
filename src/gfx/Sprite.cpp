#include "Sprite.h"
#include "vertex.h"
#include "gfx.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
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

	glm::mat4 m = transform;

	m *= glm::translate(position.x, position.y, 0.0f);
	m *= glm::rotate(rotation, 0.0f, 0.0f, 1.0f);
	m *= glm::scale(scale.x, scale.y, 1.0f);

	for (size_t i = 0; i < 4; i++)
	{
		glm::vec4 pos = m * glm::vec4(vertex[i].x, vertex[i].y, 0.0f, 1.0f);

		vertex[i].x = pos.x;
		vertex[i].y = pos.y;
	}
}

} // gfx
