#pragma once

#include "Color.h"
#include <glm/glm.hpp>

namespace gfx {

class Texture;
struct SpriteVertex;

class Sprite
{
public:
	Sprite();

	Color color;
	Texture *texture;

	float width;
	float height;
	float rotation;

	glm::vec2 tx0, tx1;
	glm::vec2 position;
	glm::vec2 center;
	glm::vec2 scale;
	glm::mat4 transform;

	void vertices(SpriteVertex (&vertex)[4]) const;
};

} // gfx
