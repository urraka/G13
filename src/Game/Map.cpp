#include "Map.h"
#include "Game.h"
#include "Debugger.h"

#include <gfx/gfx.h>
#include <set>

static std::vector< std::vector<ivec2> > line_strips();

Map::Map()
{
}

Map::~Map()
{
	for (size_t i = 0; i < vbos_.size(); i++)
	{
		delete vbos_[i]->ibo();
		delete vbos_[i];
	}
}

void Map::load()
{
	std::vector< std::vector<ivec2> > lineStrips = line_strips();
	collisionMap_.create(lineStrips);

	std::vector<vec2> polygon;
	std::vector<gfx::ColorVertex> vert;

	// first line strip is map bounds, so i'll replace it with a new strip that makes a polygon around it

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

	for (size_t iStrip = 0; iStrip < lineStrips.size(); iStrip++)
	{
		const std::vector<ivec2> &strip = lineStrips[iStrip];
		std::vector<vec2> polygon(strip.size() - 1);

		vert.resize(polygon.size());

		for (size_t i = 0; i < strip.size() - 1; i++)
		{
			polygon[i] = vec2((float)strip[i].x, (float)strip[i].y);

			vert[i].x = polygon[i].x;
			vert[i].y = polygon[i].y;
			vert[i].r = 0;
			vert[i].g = 0;
			vert[i].b = 0;
			vert[i].a = 255;
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

const Collision::Map *Map::collisionMap() const
{
	return &collisionMap_;
}

void Map::draw()
{
	DBG(gfx::wireframe(dbg->wireframe));

	for (size_t i = 0; i < vbos_.size(); i++)
		gfx::draw(vbos_[i]);

	DBG(gfx::wireframe(false));
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
