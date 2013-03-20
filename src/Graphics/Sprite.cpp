#include <pch.h>
#include <Graphics/Graphics.h>

Sprite::Sprite()
	:	angle(0.0f)
{
}

Geometry Sprite::geometry()
{
	const size_t nVertices = 4;
	const size_t nIndices = 6;

	vertices_[0].position = vec2(0.0f);
	vertices_[1].position = vec2(size.x, 0.0f);
	vertices_[2].position = size;
	vertices_[3].position = vec2(0.0f, size.y);

	vertices_[0].uv = vec2(texcoords[0], texcoords[1]);
	vertices_[1].uv = vec2(texcoords[2], texcoords[1]);
	vertices_[2].uv = vec2(texcoords[2], texcoords[3]);
	vertices_[3].uv = vec2(texcoords[0], texcoords[3]);

	mat4 transform = glm::translate(position.x, position.y, 0.0f) * glm::rotate(angle, 0.0f, 0.0f, 1.0f) * glm::translate(-center.x, -center.y, 0.0f);

	for (size_t i = 0; i < nVertices; i++)
		vertices_[i].position = glm::vec2(transform * vec4(vertices_[i].position, 0.0f, 1.0f));

	indices_[0] = 0;
	indices_[1] = 1;
	indices_[2] = 2;
	indices_[3] = 2;
	indices_[4] = 3;
	indices_[5] = 0;

	return Geometry(nVertices, vertices_, nIndices, indices_);
}
