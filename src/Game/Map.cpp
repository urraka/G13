#include <Game/Game.h>
#include <Game/Map.h>

#include <set>

Map::Map()
{
}

Map::~Map()
{
	for (size_t i = 0; i < buffers_.size(); i++)
		delete buffers_[i];
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
	std::vector<uint16_t> trianglesIndices = math::triangulate(polygon);

	std::vector<uint16_t> linesIndices;
	linesIndices.reserve(trianglesIndices.size() * 2);

	for (size_t i = 0; i < polygon.size(); i++)
	{
		vertices[i].position = polygon[i];
		vertices[i].color = u8vec4(0, 0x30, 0, 255);
	}

	struct line_t
	{
		uint16_t a;
		uint16_t b;
		uint32_t pack()
		{
			uint32_t x = a;
			uint32_t y = b;
			return x < y ? (a << 16) | b : a | (b << 16);
		};
	};

	std::set<uint32_t> lines;

	for (size_t i = 0; i < trianglesIndices.size(); i += 3)
	{
		line_t L[3];

		L[0].a = trianglesIndices[i + 0];
		L[0].b = trianglesIndices[i + 1];
		L[1].a = trianglesIndices[i + 1];
		L[1].b = trianglesIndices[i + 2];
		L[2].a = trianglesIndices[i + 2];
		L[2].b = trianglesIndices[i + 0];

		for (int j = 0; j < 3; j++)
		{
			if (lines.insert(L[j].pack()).second)
			{
				linesIndices.push_back(L[j].a);
				linesIndices.push_back(L[j].b);
			}
		}
	}

	std::cout << "lines.size() = " << lines.size() << std::endl;
	std::cout << "linesIndices.size() = " << linesIndices.size() << std::endl;

	buffers_.resize(2);
	buffers_[0] = graphics->buffer<ColorVertex>(vbo_t::Triangles, vbo_t::StaticDraw, vbo_t::StaticDraw, vertices.size(), trianglesIndices.size());
	buffers_[0]->set(vertices.data(), 0, vertices.size());
	buffers_[0]->set(trianglesIndices.data(), 0, trianglesIndices.size());

	for (size_t i = 0; i < polygon.size(); i++)
		vertices[i].color = u8vec4(0, 0, 0, 255);

	buffers_[1] = graphics->buffer<ColorVertex>(vbo_t::Lines, vbo_t::StaticDraw, vbo_t::StaticDraw, vertices.size(), linesIndices.size());
	buffers_[1]->set(vertices.data(), 0, vertices.size());
	buffers_[1]->set(linesIndices.data(), 0, linesIndices.size());
}

void Map::draw(Graphics *graphics)
{
	graphics->bind(Graphics::ColorShader);
	graphics->draw(buffers_[0]);
	graphics->draw(buffers_[1]);
}
