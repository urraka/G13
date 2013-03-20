#pragma once

class Sprite
{
public:
	Sprite();
	Geometry geometry();

	Texture *texture;
	vec4 texcoords;
	vec2 position;
	vec2 center;
	vec2 size;
	float angle;

private:
	Vertex vertices_[4];
	uint16_t indices_[6];
};
