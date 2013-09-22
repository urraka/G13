#include "g13.h"
#include "Map.h"

#include <math/triangulate.h>
#include <gfx/gfx.h>
#include <json/json.h>
#include <hlp/read.h>

namespace g13 {

Map::Map()
	:	vbo_(0),
		ground_(0),
		outlines_(0),
		diagram_(0),
		background_(0),
		colorLocation_(-1)
{
}

Map::~Map()
{
	if (vbo_      != 0) delete vbo_;
	if (ground_   != 0) delete ground_;
	if (outlines_ != 0) delete outlines_;
	if (diagram_  != 0) delete diagram_;
	if (background_  != 0) delete background_;
}

void Map::load()
{
	using std::vector;

	Json::Value data;
	Json::Reader json(Json::Features::strictMode());
	json.parse(hlp::read("data/map2.json"), data);

	vector<gfx::SimpleVertex> vertices;

	int tl = 0;
	int tr = 0;
	int bl = 0;
	int br = 0;

	// get all vertices and create main vbo

	{
		Json::Value &vertexData = data["vertices"];

		vertices.resize(vertexData.size());

		for (int i = 0; i < (int)vertexData.size(); i++)
		{
			vertices[i].x = vertexData[i]["x"].asFloat() * 2.0f;
			vertices[i].y = vertexData[i]["y"].asFloat() * 2.0f;
		}

		if (vbo_ != 0)
			delete vbo_;

		vbo_ = new gfx::VBO();
		vbo_->allocate<gfx::SimpleVertex>(vertices.size(), gfx::Static);
		vbo_->set(vertices.data(), 0, vertices.size());
	}

	// create collision map

	{
		typedef vector<glm::ivec2> LineStrip;

		Json::Value &outlines = data["outlines"];
		vector<LineStrip> linestrips(outlines.size());

		debug_log("Number of outlines: " << outlines.size());

		for (int i = 0; i < (int)outlines.size(); i++)
		{
			LineStrip &strip = linestrips[i];
			Json::Value &outline = outlines[i];

			strip.resize(outline.size() + 1);

			for (int j = 0; j < (int)outline.size(); j++)
			{
				int index = outline[j].asInt();

				strip[j].x = std::floor(vertices[index].x);
				strip[j].y = std::floor(vertices[index].y);
			}

			strip[strip.size() - 1] = strip[0];
		}

		collisionMap_.create(linestrips);
	}

	// create ground ibo and calculate bound indices on the way

	{
		Json::Value &polygonsData = data["polygons"];

		vector< vector<uint16_t> > indicesList(polygonsData.size());

		int count = 0;

		for (int i = 0; i < (int)polygonsData.size(); i++)
		{
			Json::Value &polygonData = polygonsData[i];
			vector<vec2> polygon(polygonData.size());

			if (i == 0)
			{
				int index = polygonData[0].asInt();

				tl = index;
				tr = index;
				bl = index;
				br = index;
			}

			for (int j = 0; j < (int)polygonData.size(); j++)
			{
				int index = polygonData[j].asInt();

				polygon[j].x = vertices[index].x;
				polygon[j].y = vertices[index].y;

				if (polygon[j].x < vertices[tl].x || polygon[j].y < vertices[tl].y) tl = index;
				if (polygon[j].x > vertices[tr].x || polygon[j].y < vertices[tr].y) tr = index;
				if (polygon[j].x < vertices[bl].x || polygon[j].y > vertices[bl].y) bl = index;
				if (polygon[j].x > vertices[br].x || polygon[j].y > vertices[br].y) br = index;
			}

			indicesList[i] = math::triangulate<uint16_t>(polygon);

			if (indicesList[i].size() == 0)
				debug_log("triangulation failed for polygon " << i << " with " << polygon.size() << " vertices");

			for (int j = 0; j < (int)indicesList[i].size(); j++)
				indicesList[i][j] = polygonData[indicesList[i][j]].asInt();

			count += indicesList[i].size();
		}

		if (ground_ != 0)
			delete ground_;

		ground_ = new gfx::IBO(count, gfx::Static);

		int offset = 0;

		for (int i = 0; i < (int)indicesList.size(); i++)
		{
			ground_->set(indicesList[i].data(), offset, indicesList[i].size());
			offset += indicesList[i].size();
		}
	}

	// create diagram ibo

	{
		Json::Value &edges = data["diagram"]["edges"];

		vector<uint16_t> indices(edges.size() * 2);

		for (int i = 0; i < (int)edges.size(); i++)
		{
			Json::Value &edge = edges[i];

			indices[i * 2 + 0] = edge["a"].asInt();
			indices[i * 2 + 1] = edge["b"].asInt();
		}

		if (diagram_ != 0)
			delete diagram_;

		diagram_ = new gfx::IBO(indices.size(), gfx::Static);
		diagram_->set(indices.data(), 0, indices.size());
	}

	// create outlines ibo

	{
		Json::Value &outlines = data["outlines"];

		vector<uint16_t> indices;

		for (int i = 0; i < (int)outlines.size(); i++)
		{
			Json::Value &outline = outlines[i];

			for (int j = 0; j < (int)outline.size() - 1; j++)
			{
				indices.push_back(outline[j].asInt());
				indices.push_back(outline[j + 1].asInt());
			}

			indices.push_back(outline[(int)outline.size() - 1].asInt());
			indices.push_back(outline[0].asInt());
		}

		// bounding outline

		indices.push_back(tl);
		indices.push_back(tr);
		indices.push_back(tr);
		indices.push_back(br);
		indices.push_back(br);
		indices.push_back(bl);
		indices.push_back(bl);
		indices.push_back(tl);

		if (outlines_ != 0)
			delete outlines_;

		outlines_ = new gfx::IBO(indices.size(), gfx::Static);
		outlines_->set(indices.data(), 0, indices.size());
	}

	// create background ibo

	{
		if (background_ != 0)
			delete background_;

		uint16_t indices[4];

		indices[0] = tl;
		indices[1] = tr;
		indices[2] = br;
		indices[3] = bl;

		background_ = new gfx::IBO(4, gfx::Static);
		background_->set(indices, 0, 4);
	}

	gfx::Shader *shader = gfx::default_shader<gfx::SimpleVertex>();
	colorLocation_ = shader->location("color");
}

void Map::draw()
{
	gfx::Shader *shader = gfx::default_shader<gfx::SimpleVertex>();

	float lineWidth = gfx::line_width();

	gfx::line_width(1.0f);

	shader->uniform(colorLocation_, glm::vec4(0.5f, 0.5f, 1.0f, 1.0f));
	vbo_->ibo(background_);
	vbo_->mode(gfx::TriangleFan);
	gfx::draw(vbo_);

	#ifdef DEBUG
		gfx::wireframe(dbg->wireframe);
	#endif

	shader->uniform(colorLocation_, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	vbo_->ibo(ground_);
	vbo_->mode(gfx::Triangles);
	gfx::draw(vbo_);

	#ifdef DEBUG
		gfx::wireframe(false);

		if (!dbg->wireframe)
		{
	#endif

	shader->uniform(colorLocation_, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
	vbo_->ibo(diagram_);
	vbo_->mode(gfx::Lines);
	gfx::draw(vbo_);

	shader->uniform(colorLocation_, glm::vec4(0.0f, 1.0f, 0.0f, 0.5f));
	vbo_->ibo(ground_);
	vbo_->mode(gfx::Triangles);
	gfx::draw(vbo_);

	shader->uniform(colorLocation_, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	vbo_->ibo(outlines_);
	vbo_->mode(gfx::Lines);
	gfx::line_width(3.0f);
	gfx::draw(vbo_);

	#ifdef DEBUG
		}
	#endif

	gfx::line_width(lineWidth);
}

const Collision::Map *Map::collisionMap() const
{
	return &collisionMap_;
}

} // g13

/*
#include <g13/res.h>
#include <g13/math.h>
#include <math/triangulate.h>
#include <glm/gtc/random.hpp>
#include <gfx/gfx.h>
#include <json/json.h>
#include <hlp/read.h>
#include <algorithm>

namespace g13
{

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

Map::Map() : rocks_(0), trees_(0)
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

	if (trees_ != 0)
		delete trees_;
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

		// create a tree spritebatch

		if (0)
		{
			std::vector<gfx::Sprite> sprites(floorstrips.size());

			gfx::Texture *texture = res::texture(res::Tree);

			for (size_t i = 0; i < floorstrips.size(); i++)
			{
				const LineStrip &strip = floorstrips[i];

				const int index = rand() % (strip.size() - 1);

				const vec2 &a = strip[index + 0];
				const vec2 &b = strip[index + 1];

				const vec2 n = normal(a, b);
				const float angle = glm::atan(n.y, n.x) + M_PI / 2.0f;

				sprites[i].position = glm::mix(a, b, glm::linearRand(0.0f, 1.0f));
				sprites[i].width = texture->width();
				sprites[i].height = texture->height();
				sprites[i].center = vec2(168.0f, 252.0f);
				sprites[i].color = gfx::Color(0x66);
				sprites[i].scale = vec2(glm::linearRand(0.7f, 1.0f));
				sprites[i].rotation = angle;// + glm::linearRand(-10.0f, 10.0f);
			}

			trees_ = new gfx::SpriteBatch(sprites.size(), gfx::Static);
			trees_->texture(texture);
			trees_->add(sprites.data(), sprites.size());
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

			// create the sprite batch

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
				rocks[i].rotation = glm::linearRand(-M_PI, M_PI);
				rocks[i].scale = vec2(0.15f);
			}

			rocks_ = new gfx::SpriteBatch(rocks.size(), gfx::Static);
			rocks_->texture(res::texture(res::Rocks));
			rocks_->add(rocks.data(), rocks.size());

			if (sheet != 0)
				delete[] sheet;
		}

		// convert each floorstrip into a platform

		if (0)
		{
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
	}

	// triangulate line strips and create vbos

	{
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

		// triangulate polygons and fill vbo/ibo

		{
			struct section_t
			{
				std::vector<vec2> polygon;
				std::vector<uint16_t> indices;
			};

			const size_t nSections = lineStrips.size();
			section_t *sections = new section_t[nSections];

			uint16_t nVertices = 0;
			size_t nIndices = 0;

			for (size_t i = 0; i < nSections; i++)
			{
				section_t &section = sections[i];
				const std::vector<ivec2> &strip = lineStrips[i];

				section.polygon.resize(strip.size() - 1);

				for (size_t j = 0; j < strip.size() - 1; j++)
					section.polygon[j] = vec2(strip[j]);

				section.indices = math::triangulate(section.polygon);

				for (size_t j = 0; j < section.indices.size(); j++)
					section.indices[j] += nVertices;

				nVertices += section.polygon.size();
				nIndices += section.indices.size();
			}

			const gfx::Color color(0);

			std::vector<gfx::ColorVertex> vertices(nVertices);
			std::vector<uint16_t> indices(nIndices);

			size_t iv = 0;
			size_t ii = 0;

			for (size_t i = 0; i < nSections; i++)
			{
				const section_t &s = sections[i];

				for (size_t j = 0; j < s.polygon.size(); j++)
					vertices[iv + j] = gfx::color_vertex(s.polygon[j].x, s.polygon[j].y, color);

				for (size_t j = 0; j < s.indices.size(); j++)
					indices[ii + j] = s.indices[j];

				iv += s.polygon.size();
				ii += s.indices.size();
			}

			delete[] sections;

			gfx::IBO *ibo = new gfx::IBO(indices.size(), gfx::Static);
			ibo->set(indices.data(), 0, indices.size());

			gfx::VBO *vbo = new gfx::VBO(ibo);
			vbo->allocate<gfx::ColorVertex>(vertices.size(), gfx::Static);
			vbo->set(vertices.data(), 0, vertices.size());

			vbos_.push_back(vbo);
		}

		if (0)
		{
			const gfx::Color color(0x00);

			std::vector<vec2> polygon;
			std::vector<gfx::ColorVertex> vert;

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
	if (trees_ != 0)
		gfx::draw(trees_);

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
*/