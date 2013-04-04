#include <Game/Game.h>
#include <Game/Map.h>

Map::Map() : buffer_(0)
{
}

Map::~Map()
{
}

void Map::load(Graphics *graphics)
{
	ColorVertex vertices[6];

	vertices[0].position = vec2(-200.0f, 0.0f);
	vertices[1].position = vec2(-100.0f, -100.0f);
	vertices[2].position = vec2(100.0f, -100.0f);
	vertices[3].position = vec2(200.0f, 0.0f);
	vertices[4].position = vec2(100.0f, 100.0f);
	vertices[5].position = vec2(-100.0f, -100.0f);

	vertices[0].color = u8vec4(0, 0, 0, 255);
	vertices[1].color = u8vec4(0, 0, 0, 255);
	vertices[2].color = u8vec4(0, 0, 0, 255);
	vertices[3].color = u8vec4(0, 0, 0, 255);
	vertices[4].color = u8vec4(0, 0, 0, 255);
	vertices[5].color = u8vec4(0, 0, 0, 255);

	buffer_ = graphics->buffer<ColorVertex>(VBO<ColorVertex>::TriangleFan, VBO<ColorVertex>::StaticDraw, 7);
	buffer_->set(vertices, 0, 6);
}

void Map::draw(Graphics *graphics)
{
	graphics->bind(Graphics::ColorShader);
	graphics->draw(buffer_);
}
