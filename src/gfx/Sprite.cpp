#include "Sprite.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <stdint.h>

namespace gfx {

Sprite::Sprite()
	:	x(0.0f),
		y(0.0f),
		width(0.0f),
		height(0.0f),
		cx(0.0f),
		cy(0.0f),
		sx(1.0f),
		sy(1.0f),
		angle(0.0f),
		u(),
		v(),
		opacity(1.0f),
		texture(0)
{
}

void Sprite::vertices(SpriteVertex (&vertex)[4]) const
{
	// opacity

	uint8_t alpha = (uint8_t)(opacity * 255.0f);

	vertex[0].opacity = alpha;
	vertex[1].opacity = alpha;
	vertex[2].opacity = alpha;
	vertex[3].opacity = alpha;

	// texcords

	vertex[0].u = u[0];
	vertex[0].v = v[0];
	vertex[1].u = u[1];
	vertex[1].v = v[0];
	vertex[2].u = u[1];
	vertex[2].v = v[1];
	vertex[3].u = u[0];
	vertex[3].v = v[1];

	// position

	vertex[0].x = -cx;
	vertex[0].y = -cy;
	vertex[1].x = -cx + width;
	vertex[1].y = -cy;
	vertex[2].x = -cx + width;
	vertex[2].y = -cy + height;
	vertex[3].x = -cx;
	vertex[3].y = -cy + height;

	// transform the vertices

	glm::mat4 transform = glm::translate(x, y, 0.0f) *
		glm::rotate(angle, 0.0f, 0.0f, 1.0f) *
		glm::scale(sx, sy, 1.0f);

	for (size_t i = 0; i < 4; i++)
	{
		glm::vec4 pos = transform * glm::vec4(vertex[i].x, vertex[i].y, 0.0f, 1.0f);

		vertex[i].x = pos.x;
		vertex[i].y = pos.y;
	}
}

} // gfx
