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

	std::vector<Triangle> triangles = math::triangulate(polygon);

	ColorVertex vertices[3];
	vertices[0].color = u8vec4(0, 0, 0, 255);
	vertices[1].color = u8vec4(0, 0, 0, 255);
	vertices[2].color = u8vec4(0, 0, 0, 255);

	buffer_ = graphics->buffer<ColorVertex>(VBO<ColorVertex>::Triangles, VBO<ColorVertex>::StaticDraw, 3 * triangles.size());

	for (size_t i = 0; i < triangles.size(); i++)
	{
		vertices[0].position = triangles[i].a;
		vertices[1].position = triangles[i].b;
		vertices[2].position = triangles[i].c;

		buffer_->set(vertices, 3 * i, 3);
	}
}

void Map::draw(Graphics *graphics)
{
	graphics->bind(Graphics::ColorShader);
	graphics->draw(buffer_);
}
