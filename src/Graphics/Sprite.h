#pragma once

class Sprite
{
public:
	Sprite();
	void vertices(Vertex (&vertices)[4]) const;

	Texture *texture;
	vec4 texcoords;
	vec2 position;
	vec2 center;
	vec2 size;
	vec2 scale;
	float angle;
};
