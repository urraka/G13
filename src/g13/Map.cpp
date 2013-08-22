#include "g13.h"
#include "Map.h"

#include <g13/res.h>
#include <g13/math.h>
#include <math/triangulate.h>
#include <glm/gtc/random.hpp>
#include <gfx/gfx.h>
#include <json/json.h>
#include <hlp/read.h>
#include <algorithm>

namespace g13 {

typedef glm::ivec2 ivec2;

static std::vector< std::vector<ivec2> > line_strips();

static inline float slope(const vec2 &a, const vec2 &b)
{
	return (a.y - b.y) / (a.x - b.x);
}

static inline vec2 normal(const vec2 &a, const vec2 &b)
{
	return glm::normalize(vec2(-(b.y - a.y), b.x - a.x));
}

static inline bool is_floor(const vec2 &a, const vec2 &b)
{
	return a.x != b.x &&
		glm::abs(slope(a, b)) <= 2.0f &&
		glm::dot(vec2(0.0f, -1.0f), normal(a, b)) > 0.0f;
}

static inline bool is_floor(const ivec2 &a, const ivec2 &b)
{
	return is_floor(vec2(a), vec2(b));
}

Map::Map() : rocks_(0)
{
}

Map::~Map()
{
	for (size_t i = 0; i < vbos_.size(); i++)
	{
		delete vbos_[i]->ibo();
		delete vbos_[i];
	}

	if (rocks_ != 0)
		delete rocks_;
}

void Map::load()
{
	std::vector< std::vector<ivec2> > lineStrips = line_strips();
	collisionMap_.create(lineStrips);

	// find strips which are floors and create "platforms"

	std::vector<gfx::VBO*> vboPlatforms;

	{
		typedef std::vector<vec2>      LineStrip;
		typedef std::vector<LineStrip> LineStripArray;

		// find all floor strips

		LineStripArray floorstrips;

		for (size_t iStrip = 0; iStrip < lineStrips.size(); iStrip++)
		{
			const std::vector<ivec2> &strip = lineStrips[iStrip];

			const size_t N = strip.size();
			const size_t is_loop = (strip[0] == strip[N - 1] ? 1 : 0);

			// find a starting point

			size_t first = 0;

			if (is_loop == 1)
			{
				while (first < N && is_floor(strip[first], strip[(first + 1) % N]))
					first++;
			}

			// fill floorstrips vector

			int hook = -1;

			for (size_t i = 0; i < N - 1; i++)
			{
				const ivec2 &a = strip[(first + i) % (N - is_loop)];
				const ivec2 &b = strip[(first + i + 1) % (N - is_loop)];

				if (!is_floor(a, b))
				{
					hook = -1;
					continue;
				}

				if (hook == -1)
				{
					hook = i;

					floorstrips.push_back(std::vector<vec2>());
					floorstrips.back().push_back(vec2(a));
				}

				floorstrips.back().push_back(vec2(b));
			}
		}

		// find a bunch of points where rock sprites will be placed

		std::vector<vec2> rockpoints;

		for (size_t i = 0; i < floorstrips.size(); i++)
		{
			for (size_t j = 0; j < floorstrips[i].size() - 1; j++)
			{
				const vec2 &a = floorstrips[i][j];
				const vec2 &b = floorstrips[i][j + 1];

				const float desiredDistance = 10.0f;

				const float length = glm::length(b - a);
				const int N = (int)glm::floor(length / desiredDistance);
				const vec2 v = ((b - a) / length) * (length / N);

				vec2 p = a;

				for (int k = 0; k < N; k++, p += v)
					rockpoints.push_back(p);
			}

			rockpoints.push_back(floorstrips[i].back());
		}

		std::random_shuffle(rockpoints.begin(), rockpoints.end());

		std::cout << "Rock count: " << rockpoints.size() << std::endl;

		// create a sprite batch for the rocks

		{
			// first load rock spritesheet information

			struct sprite_t
			{
				float width, height;
				vec2 tx0, tx1;
			};

			sprite_t *sheet = 0;
			int sheetsize = 0;

			{
				Json::Value root;

				Json::Reader json(Json::Features::strictMode());
				json.parse(hlp::read("data/rocks.json"), root);

				int width = root["width"].asInt();
				int height = root["height"].asInt();

				const Json::Value &list = root["sprites"];

				sheetsize = list.size();

				if (sheetsize > 0)
					sheet = new sprite_t[sheetsize];

				for (int i = 0; i < sheetsize; i++)
				{
					const Json::Value &sprite = list[i];

					float x = sprite["x"].asFloat();
					float y = sprite["y"].asFloat();

					sheet[i].width = sprite["width"].asFloat();
					sheet[i].height = sprite["height"].asFloat();

					sheet[i].tx0.x = x / width;
					sheet[i].tx0.y = y / height;
					sheet[i].tx1.x = (x + sheet[i].width) / width;
					sheet[i].tx1.y = (y + sheet[i].height) / height;
				}
			}

			std::vector<gfx::Sprite> rocks(rockpoints.size());

			for (size_t i = 0; i < rockpoints.size(); i++)
			{
				sprite_t &sprite = sheet[rand() % sheetsize];

				rocks[i].position = rockpoints[i];
				rocks[i].width = sprite.width;
				rocks[i].height = sprite.height;
				rocks[i].tx0 = sprite.tx0;
				rocks[i].tx1 = sprite.tx1;
				rocks[i].center = vec2(sprite.width, sprite.height) / 2.0f;
				// rocks[i].color = gfx::Color(0xCC);
				rocks[i].rotation = glm::linearRand(-180.0f, 180.0f);
				rocks[i].scale = vec2(0.15f);
			}

			rocks_ = new gfx::SpriteBatch(rocks.size());
			rocks_->texture(res::texture(res::Rocks));
			rocks_->add(rocks.data(), rocks.size());

			if (sheet != 0)
				delete[] sheet;
		}

		// convert each floorstrip into a platform

		LineStripArray platforms(floorstrips.size());

		for (size_t i = 0; i < floorstrips.size(); i++)
		{
			const LineStrip &strip = floorstrips[i];

			const size_t N = strip.size();

			LineStrip &platform = platforms[i];
			platform.resize(N * 2);

			const float offset = 20.0f;

			for (size_t j = 0; j < N; j++)
				platform[j] = strip[j] - vec2(0.0f, offset);

			for (size_t j = 0; j < N; j++)
				platform[N + j] = strip[(N - j - 1)] + vec2(0.0f, offset);
		}

		// triangulate platforms and create a vbo for each one

		if (0)
		{
			const gfx::Color color(200);

			std::vector<gfx::ColorVertex> vertices;

			for (size_t i = 0; i < platforms.size(); i++)
			{
				const LineStrip &platform = platforms[i];

				vertices.resize(platform.size());

				for (size_t j = 0; j < platform.size(); j++)
					vertices[j] = gfx::color_vertex(platform[j].x, platform[j].y, color);

				std::vector<uint16_t> indices = math::triangulate(platforms[i]);

				gfx::IBO *ibo = new gfx::IBO(indices.size(), gfx::Static);
				ibo->set(indices.data(), 0, indices.size());

				gfx::VBO *vbo = new gfx::VBO(ibo);
				vbo->allocate<gfx::ColorVertex>(vertices.size(), gfx::Static);
				vbo->set(vertices.data(), 0, vertices.size());

				vboPlatforms.push_back(vbo);
			}
		}

		// add strips as lines

		if (0)
		{
			const LineStripArray &strips = platforms;

			size_t nVertices = 0;
			size_t nIndices = 0;

			for (size_t i = 0; i < strips.size(); i++)
			{
				nVertices += strips[i].size();
				nIndices += 2 * (strips[i].size() - 1);
			}

			std::vector<gfx::ColorVertex> vertices(nVertices);
			std::vector<uint16_t> indices(nIndices);

			size_t iv = 0;
			size_t ii = 0;

			for (size_t i = 0; i < strips.size(); i++)
			{
				const gfx::Color color(0, 0, 255);

				vertices[iv++] = gfx::color_vertex(strips[i][0].x, strips[i][0].y, color);

				for (size_t j = 1; j < strips[i].size(); j++)
				{
					vertices[iv++] = gfx::color_vertex(strips[i][j].x, strips[i][j].y, color);

					indices[ii++] = iv - 2;
					indices[ii++] = iv - 1;
				}
			}

			gfx::IBO *ibo = new gfx::IBO(indices.size(), gfx::Static);
			gfx::VBO *vbo = new gfx::VBO(ibo);

			ibo->set(indices.data(), 0, indices.size());
			vbo->allocate<gfx::ColorVertex>(vertices.size(), gfx::Static);
			vbo->set(vertices.data(), 0, vertices.size());
			vbo->mode(gfx::Lines);

			vboPlatforms.push_back(vbo);
		}
	}

	// triangulate line strips and create vbos

	{
		std::vector<vec2> polygon;
		std::vector<gfx::ColorVertex> vert;

		// first line strip is map bounds, replace it with a new strip that makes a polygon around it
		// note: this part actually modifies lineStrips[0]

		{
			std::vector<ivec2> firstStrip;

			const std::vector<ivec2> &strip = lineStrips[0];

			size_t leftmost = 0;
			ivec2 tl = strip[0];
			ivec2 br = strip[0];

			const size_t sz = strip.size();

			for (size_t i = 1; i < sz - 1; i++)
			{
				if (strip[i].x < tl.x) tl.x = strip[leftmost = i].x;
				if (strip[i].x > br.x) br.x = strip[i].x;
				if (strip[i].y < tl.y) tl.y = strip[i].y;
				if (strip[i].y > br.y) br.y = strip[i].y;
			}

			firstStrip.resize(sz + 7);

			for (size_t i = 0; i < sz - 1; i++)
				firstStrip[i] = strip[(leftmost + i) % (sz - 1)];

			firstStrip[sz - 1] = firstStrip[0];

			const float padding = 2000.0f;

			firstStrip[sz + 0] = vec2(tl.x - padding, firstStrip[0].y);
			firstStrip[sz + 1] = vec2(tl.x - padding, br.y + padding);
			firstStrip[sz + 2] = vec2(br.x + padding, br.y + padding);
			firstStrip[sz + 3] = vec2(br.x + padding, tl.y - padding);
			firstStrip[sz + 4] = vec2(tl.x - padding, tl.y - padding);
			firstStrip[sz + 5] = vec2(tl.x - padding, firstStrip[0].y);
			firstStrip[sz + 6] = vec2(firstStrip[0].x, firstStrip[0].y);

			lineStrips[0].swap(firstStrip);
		}

		// the rest of the strips are normal polygons

		const gfx::Color color(0x00);

		for (size_t iStrip = 0; iStrip < lineStrips.size(); iStrip++)
		{
			const std::vector<ivec2> &strip = lineStrips[iStrip];
			std::vector<vec2> polygon(strip.size() - 1);

			vert.resize(polygon.size());

			for (size_t i = 0; i < strip.size() - 1; i++)
			{
				polygon[i] = vec2((float)strip[i].x, (float)strip[i].y);
				vert[i] = gfx::color_vertex(polygon[i].x, polygon[i].y, color);
			}

			std::vector<uint16_t> indices = math::triangulate(polygon);

			gfx::IBO *ibo = new gfx::IBO(indices.size(), gfx::Static);
			ibo->set(indices.data(), 0, indices.size());

			gfx::VBO *vbo = new gfx::VBO(ibo);
			vbo->allocate<gfx::ColorVertex>(vert.size(), gfx::Static);
			vbo->set(vert.data(), 0, vert.size());

			vbos_.push_back(vbo);
		}
	}

	for (size_t i = 0; i < vboPlatforms.size(); i++)
		vbos_.push_back(vboPlatforms[i]);
}

const Collision::Map *Map::collisionMap() const
{
	return &collisionMap_;
}

void Map::draw()
{
	#ifdef DEBUG
		gfx::wireframe(dbg->wireframe);
	#endif

	for (size_t i = 0; i < vbos_.size(); i++)
		gfx::draw(vbos_[i]);

	#ifdef DEBUG
		gfx::wireframe(false);
	#endif

	gfx::draw(rocks_);
}

std::vector< std::vector<ivec2> > line_strips()
{
	std::vector< std::vector<ivec2> > lineStrips(10);

	lineStrips[0].push_back(ivec2(-151, -73));
	lineStrips[0].push_back(ivec2(287, -58));
	lineStrips[0].push_back(ivec2(1033, 20));
	lineStrips[0].push_back(ivec2(1360, -20));
	lineStrips[0].push_back(ivec2(1477, 442));
	lineStrips[0].push_back(ivec2(1419, 677));
	lineStrips[0].push_back(ivec2(1323, 792));
	lineStrips[0].push_back(ivec2(1333, 890));
	lineStrips[0].push_back(ivec2(1417, 1075));
	lineStrips[0].push_back(ivec2(1176, 1104));
	lineStrips[0].push_back(ivec2(949, 1030));
	lineStrips[0].push_back(ivec2(735, 1034));
	lineStrips[0].push_back(ivec2(654, 971));
	lineStrips[0].push_back(ivec2(521, 900));
	lineStrips[0].push_back(ivec2(426, 891));
	lineStrips[0].push_back(ivec2(421, 822));
	lineStrips[0].push_back(ivec2(257, 818));
	lineStrips[0].push_back(ivec2(266, 893));
	lineStrips[0].push_back(ivec2(250, 894));
	lineStrips[0].push_back(ivec2(6, 941));
	lineStrips[0].push_back(ivec2(-107, 1028));
	lineStrips[0].push_back(ivec2(-119, 1051));
	lineStrips[0].push_back(ivec2(-477, 1058));
	lineStrips[0].push_back(ivec2(-600, 935));
	lineStrips[0].push_back(ivec2(-770, -33));
	lineStrips[0].push_back(ivec2(-569, 8));
	lineStrips[0].push_back(ivec2(-151, -73));
	lineStrips[1].push_back(ivec2(479, 764));
	lineStrips[1].push_back(ivec2(564, 788));
	lineStrips[1].push_back(ivec2(667, 769));
	lineStrips[1].push_back(ivec2(839, 721));
	lineStrips[1].push_back(ivec2(834, 659));
	lineStrips[1].push_back(ivec2(724, 627));
	lineStrips[1].push_back(ivec2(631, 656));
	lineStrips[1].push_back(ivec2(479, 764));
	lineStrips[2].push_back(ivec2(220, 565));
	lineStrips[2].push_back(ivec2(105, 536));
	lineStrips[2].push_back(ivec2(-30, 568));
	lineStrips[2].push_back(ivec2(-36, 630));
	lineStrips[2].push_back(ivec2(176, 678));
	lineStrips[2].push_back(ivec2(303, 697));
	lineStrips[2].push_back(ivec2(407, 673));
	lineStrips[2].push_back(ivec2(220, 565));
	lineStrips[3].push_back(ivec2(414, 553));
	lineStrips[3].push_back(ivec2(499, 577));
	lineStrips[3].push_back(ivec2(602, 558));
	lineStrips[3].push_back(ivec2(774, 510));
	lineStrips[3].push_back(ivec2(769, 448));
	lineStrips[3].push_back(ivec2(659, 416));
	lineStrips[3].push_back(ivec2(566, 445));
	lineStrips[3].push_back(ivec2(414, 553));
	lineStrips[4].push_back(ivec2(936, 445));
	lineStrips[4].push_back(ivec2(914, 508));
	lineStrips[4].push_back(ivec2(929, 560));
	lineStrips[4].push_back(ivec2(983, 685));
	lineStrips[4].push_back(ivec2(1139, 891));
	lineStrips[4].push_back(ivec2(1237, 930));
	lineStrips[4].push_back(ivec2(1044, 380));
	lineStrips[4].push_back(ivec2(936, 445));
	lineStrips[5].push_back(ivec2(-533, 480));
	lineStrips[5].push_back(ivec2(-474, 540));
	lineStrips[5].push_back(ivec2(-391, 531));
	lineStrips[5].push_back(ivec2(-314, 490));
	lineStrips[5].push_back(ivec2(-220, 545));
	lineStrips[5].push_back(ivec2(-139, 560));
	lineStrips[5].push_back(ivec2(-151, 502));
	lineStrips[5].push_back(ivec2(-173, 467));
	lineStrips[5].push_back(ivec2(-160, 448));
	lineStrips[5].push_back(ivec2(-111, 445));
	lineStrips[5].push_back(ivec2(-30, 492));
	lineStrips[5].push_back(ivec2(13, 498));
	lineStrips[5].push_back(ivec2(30, 481));
	lineStrips[5].push_back(ivec2(-10, 462));
	lineStrips[5].push_back(ivec2(-30, 424));
	lineStrips[5].push_back(ivec2(-181, 364));
	lineStrips[5].push_back(ivec2(-251, 370));
	lineStrips[5].push_back(ivec2(-253, 321));
	lineStrips[5].push_back(ivec2(-297, 302));
	lineStrips[5].push_back(ivec2(-353, 254));
	lineStrips[5].push_back(ivec2(-430, 194));
	lineStrips[5].push_back(ivec2(-491, 197));
	lineStrips[5].push_back(ivec2(-563, 214));
	lineStrips[5].push_back(ivec2(-596, 240));
	lineStrips[5].push_back(ivec2(-533, 480));
	lineStrips[6].push_back(ivec2(-375, 745));
	lineStrips[6].push_back(ivec2(-375, 875));
	lineStrips[6].push_back(ivec2(-107, 875));
	lineStrips[6].push_back(ivec2(-375, 745));
	lineStrips[7].push_back(ivec2(733, 897));
	lineStrips[7].push_back(ivec2(1001, 897));
	lineStrips[7].push_back(ivec2(1001, 767));
	lineStrips[7].push_back(ivec2(733, 897));
	lineStrips[8].push_back(ivec2(315, 79));
	lineStrips[8].push_back(ivec2(47, 79));
	lineStrips[8].push_back(ivec2(47, 208));
	lineStrips[8].push_back(ivec2(315, 79));
	lineStrips[9].push_back(ivec2(-52, 264));
	lineStrips[9].push_back(ivec2(-52, 135));
	lineStrips[9].push_back(ivec2(-320, 135));
	lineStrips[9].push_back(ivec2(-52, 264));

	return lineStrips;
}

} // g13
