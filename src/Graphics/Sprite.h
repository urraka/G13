#pragma once

#include "../Math/math.h"
#include "Vertex.h"

class Sprite
{
public:
	Sprite();
	void vertices(TextureVertex (&vertices)[4]) const;

	vec4 texcoords;
	vec2 position;
	vec2 center;
	vec2 size;
	vec2 scale;
	float angle;
};
