#include <pch.h>
#include <Graphics/OpenGL.h>
#include <Graphics/Texture.h>
#include <Graphics/Vertex.h>
#include <Graphics/Geometry.h>
#include <Graphics/Sprite.h>

Sprite::Sprite()
	:	angle(0.0f)
{
}

Vertex *Sprite::vertices()
{
	vertices_[0].position = vec2(0.0f);
	vertices_[1].position = vec2(size.x, 0.0f);
	vertices_[2].position = size;
	vertices_[3].position = vec2(0.0f, size.y);

	vertices_[0].uv = vec2(texcoords[0], texcoords[1]);
	vertices_[1].uv = vec2(texcoords[2], texcoords[1]);
	vertices_[2].uv = vec2(texcoords[2], texcoords[3]);
	vertices_[3].uv = vec2(texcoords[0], texcoords[3]);

	mat4 transform = glm::translate(position.x, position.y, 0.0f) * glm::rotate(angle, 0.0f, 0.0f, 1.0f) * glm::translate(-center.x, -center.y, 0.0f);

	for (size_t i = 0; i < kVertices; i++)
		vertices_[i].position = glm::vec2(transform * vec4(vertices_[i].position, 0.0f, 1.0f));

	return vertices_;
}
