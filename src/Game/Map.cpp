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
	std::vector<uint16_t> triangles = math::triangulate(polygon);
	std::vector<uint16_t> indices(triangles.size() * 2);

	for (size_t i = 0; i < polygon.size(); i++)
	{
		vertices[i].position = polygon[i];
		vertices[i].color = u8vec4(0, 0, i % 2 == 0 ? 0x80 : 0x20, 255);
	}

	for (size_t i = 0, j = 0; i < triangles.size(); i += 3, j += 6)
	{
		indices[j + 0] = triangles[(i + 0) % triangles.size()];
		indices[j + 1] = triangles[(i + 1) % triangles.size()];

		indices[j + 2] = triangles[(i + 1) % triangles.size()];
		indices[j + 3] = triangles[(i + 2) % triangles.size()];

		indices[j + 4] = triangles[(i + 2) % triangles.size()];
		indices[j + 5] = triangles[(i + 3) % triangles.size()];
	}

	buffer_ = graphics->buffer<ColorVertex>(vbo_t::Triangles, vbo_t::StaticDraw, vbo_t::StaticDraw, vertices.size(), triangles.size());
	buffer_->set(vertices.data(), 0, vertices.size());
	buffer_->set(triangles.data(), 0, triangles.size());
}

void Map::draw(Graphics *graphics)
{
	graphics->bind(Graphics::ColorShader);
	graphics->draw(buffer_);
}
