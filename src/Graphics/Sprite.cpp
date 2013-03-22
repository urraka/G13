#include <pch.h>
#include <Graphics/Graphics.h>

Sprite::Sprite()
	:	angle(0.0f),
		scale(1.0f)
{
}

void Sprite::vertices(Vertex (&vertices)[4]) const
{
	vertices[0].position = vec2(0.0f) - center;
	vertices[1].position = vec2(size.x, 0.0f) - center;
	vertices[2].position = size - center;
	vertices[3].position = vec2(0.0f, size.y) - center;

	vertices[0].uv = vec2(texcoords[0], texcoords[1]);
	vertices[1].uv = vec2(texcoords[2], texcoords[1]);
	vertices[2].uv = vec2(texcoords[2], texcoords[3]);
	vertices[3].uv = vec2(texcoords[0], texcoords[3]);

	mat4 transform = glm::translate(position.x, position.y, 0.0f) * glm::rotate(angle, 0.0f, 0.0f, 1.0f) * glm::scale(scale.x, scale.y, 1.0f);

	for (size_t i = 0; i < 4; i++)
		vertices[i].position = glm::vec2(transform * vec4(vertices[i].position, 0.0f, 1.0f));
}
