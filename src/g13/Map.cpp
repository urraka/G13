#include "g13.h"
#include "Map.h"
#include "res.h"

#include <g13/coll/collision.h>
#include <math/triangulate.h>
#include <glm/gtc/random.hpp>
#include <gfx/gfx.h>
#include <json/json.h>
#include <hlp/read.h>
#include <algorithm>

namespace g13 {

typedef std::vector<vec2> linestripf_t;

static std::vector<linestripf_t> find_floorstrips(const std::vector< std::vector<fixvec2> > &linestrips);

Map::Map()
	:	world_(0),
		vbo_(0),
		ground_(0),
		outlines_(0),
		diagram_(0),
		background_(0),
		rocks_(0),
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
	if (rocks_  != 0) delete rocks_;
}

void Map::load()
{
	using std::vector;

	const float SCALE = 2.0f; // arbitrary crap, needs to go away

	Json::Value data;
	Json::Reader json(Json::Features::strictMode());
	json.parse(hlp::read("data/map2.json"), data);

	vector<gfx::SimpleVertex> vertices;
	vector<linestripf_t> floorstrips;

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
			vertices[i].x = vertexData[i]["x"].asFloat() * SCALE;
			vertices[i].y = vertexData[i]["y"].asFloat() * SCALE;
		}

		if (vbo_ != 0)
			delete vbo_;

		vbo_ = new gfx::VBO();
		vbo_->allocate<gfx::SimpleVertex>(vertices.size(), gfx::Static);
		vbo_->set(vertices.data(), 0, vertices.size());
	}

	// create collision map and find floorstrips

	{
		Json::Value &outlines = data["outlines"];
		vector< vector<fixvec2> > linestrips(outlines.size());

		for (int i = 0; i < (int)outlines.size(); i++)
		{
			vector<fixvec2> &strip = linestrips[i];
			Json::Value &outline = outlines[i];

			strip.resize(outline.size() + 1);

			for (int j = 0; j < (int)outline.size(); j++)
			{
				int index = outline[j].asInt();

				strip[j].x = fixed(vertices[index].x);
				strip[j].y = fixed(vertices[index].y);
			}

			strip[strip.size() - 1] = strip[0];
		}

		Json::Value &boundsData = data["bounds"];

		fixrect bounds(
			fixed(SCALE * boundsData["left"].asFloat()),
			fixed(SCALE * boundsData["top"].asFloat()),
			fixed(SCALE * boundsData["right"].asFloat()),
			fixed(SCALE * boundsData["bottom"].asFloat())
		);

		world_ = new coll::World(bounds);
		world_->create(linestrips);

		floorstrips = find_floorstrips(linestrips);
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
			{
				debug_log("triangulation failed for polygon " << i << " with " <<
					polygon.size() << " vertices");
			}

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

	// create rocks spritebatch

	{
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

		// load rock spritesheet information

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

		gfx::Color colors[] = {
			gfx::Color(0x4C, 0x54, 0x5F),
			gfx::Color(0x78, 0x57, 0x38),
			gfx::Color(0x3A, 0x37, 0x40),
			gfx::Color(0x56, 0x5F, 0x66),
			gfx::Color(0xFA, 0xD0, 0x84),
			gfx::Color(0xDF, 0x9D, 0x61)
		};

		for (size_t i = 0; i < rockpoints.size(); i++)
		{
			sprite_t &sprite = sheet[rand() % sheetsize];

			rocks[i].position = rockpoints[i];
			rocks[i].width = sprite.width;
			rocks[i].height = sprite.height;
			rocks[i].tx0 = sprite.tx0;
			rocks[i].tx1 = sprite.tx1;
			rocks[i].center = vec2(sprite.width, sprite.height) / 2.0f;
			rocks[i].color = colors[i % countof(colors)];
			rocks[i].rotation = glm::linearRand(-M_PI, M_PI);
			rocks[i].scale = vec2(0.15f);
		}

		if (rocks_ != 0)
			delete rocks_;

		rocks_ = new gfx::SpriteBatch(rocks.size(), gfx::Static);
		rocks_->texture(res::texture(res::Rocks));
		rocks_->add(rocks.data(), rocks.size());

		if (sheet != 0)
			delete[] sheet;
	}

	gfx::Shader *shader = gfx::default_shader<gfx::SimpleVertex>();
	colorLocation_ = shader->location("color");
}

void Map::draw(const ent::Camera *camera)
{
	gfx::Shader *shader = gfx::default_shader<gfx::SimpleVertex>();

	float lineWidth = gfx::line_width();

	gfx::line_width(1.0f);

	// shader->uniform(colorLocation_, glm::vec4(0.5f, 0.5f, 1.0f, 1.0f));
	// vbo_->ibo(background_);
	// vbo_->mode(gfx::TriangleFan);
	// gfx::draw(vbo_);

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

	// shader->uniform(colorLocation_, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
	// vbo_->ibo(diagram_);
	// vbo_->mode(gfx::Lines);
	// gfx::draw(vbo_);

	shader->uniform(colorLocation_, glm::vec4(0.0f, 1.0f, 0.0f, 0.5f));
	vbo_->ibo(ground_);
	vbo_->mode(gfx::Triangles);
	gfx::draw(vbo_);

	shader->uniform(colorLocation_, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	vbo_->ibo(outlines_);
	vbo_->mode(gfx::Lines);
	gfx::line_width(3.0f);
	gfx::draw(vbo_);

	gfx::draw(rocks_);

	#ifdef DEBUG
		}
	#endif

	gfx::line_width(lineWidth);
}

std::vector<linestripf_t> find_floorstrips(const std::vector< std::vector<fixvec2> > &linestrips)
{
	std::vector<linestripf_t> floorstrips;

	for (int i = 0; i < (int)linestrips.size(); i++)
	{
		const std::vector<fixvec2> &strip = linestrips[i];

		const int N = strip.size();
		const int is_loop = (strip[0] == strip[N - 1] ? 1 : 0);

		// find a starting point

		int first = 0;

		if (is_loop == 1)
		{
			while (first < N && coll::is_floor(strip[first], strip[(first + 1) % N]))
				first++;
		}

		// fill floorstrips vector

		int hook = -1;

		for (int j = 0; j < N - 1; j++)
		{
			const fixvec2 &a = strip[(first + j) % (N - is_loop)];
			const fixvec2 &b = strip[(first + j + 1) % (N - is_loop)];

			if (!coll::is_floor(a, b))
			{
				hook = -1;
				continue;
			}

			if (hook == -1)
			{
				hook = j;

				floorstrips.push_back(std::vector<vec2>());
				floorstrips.back().push_back(from_fixed(a));
			}

			floorstrips.back().push_back(from_fixed(b));
		}
	}

	return floorstrips;
}

} // g13
