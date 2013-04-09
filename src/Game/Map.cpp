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
	std::vector<vec2> polygon(8);

	polygon[0] = vec2(-300.0f, 0.0f);
	polygon[1] = vec2(-100.0f, 100.0f);
	polygon[2] = vec2(-100.0f, 200.0f);
	polygon[3] = vec2(100.0f, 200.0f);
	polygon[4] = vec2(100.0f, 100.0f);
	polygon[5] = vec2(300.0f, 0.0f);
	polygon[6] = vec2(100.0f, -100.0f);
	polygon[7] = vec2(-100.0f, -100.0f);

	std::vector<ColorVertex> vertices(polygon.size());
	std::vector<uint16_t> indices = math::triangulate(polygon);

	for (size_t i = 0; i < polygon.size(); i++)
	{
		vertices[i].position = polygon[i];
		vertices[i].color = u8vec4(0, 0, 0, 255);
	}

	buffer_ = graphics->buffer<ColorVertex>(VBO<ColorVertex>::Triangles, VBO<ColorVertex>::StaticDraw, VBO<ColorVertex>::StaticDraw, vertices.size(), indices.size());
	buffer_->set(vertices.data(), 0, vertices.size());
	buffer_->set(indices.data(), 0, indices.size());
}

void Map::draw(Graphics *graphics)
{
	graphics->bind(Graphics::ColorShader);
	graphics->draw(buffer_);
}
