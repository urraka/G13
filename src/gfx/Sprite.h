#pragma once

#include "vertex.h"

namespace gfx {

class Texture;

class Sprite
{
public:
	Sprite();

	float x, y;
	float width;
	float height;
	float cx, cy; // center
	float sx, sy; // scale
	float angle;
	float u[2], v[2]; // texcoords; 0: top-left, 1: bottom-right
	float opacity;

	Texture *texture;

	void vertices(SpriteVertex (&vertex)[4]) const;
};

} // gfx
