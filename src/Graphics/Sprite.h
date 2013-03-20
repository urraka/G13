#pragma once

class Sprite
{
public:
	Sprite();
	Vertex *vertices();

	Texture *texture;
	vec4 texcoords;
	vec2 position;
	vec2 center;
	vec2 size;
	float angle;

	enum
	{
		kVertices = 4,
		kIndices = 6
	};

private:
	Vertex vertices_[4];
};
