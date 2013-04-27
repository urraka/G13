#include <Game/Collision.h>
#include <iostream>
#include <iomanip>
#include <sstream>

#define DEBUG_COLLISION_DATA 0

Collision::Hull::Hull() : nodes() {}

Collision::Hull::Hull(const Collision::Hull &hull)
{
	*this = hull;
}

Collision::Hull& Collision::Hull::operator= (const Hull &hull)
{
	nodes[0] = hull.nodes[0];
	nodes[1] = hull.nodes[1];
	nodes[2] = hull.nodes[2];

	// nodes[0] is the main node
	// nodes[0] will usually be linked in this way:
	//     nodes[1] <- prev <- nodes[0] -> next -> nodes[2]
	// in some cases nodes[0] will be linked to a "real" node (node from the collision map),
	// drawing either nodes[1] or nodes[2] irrelevant

	nodes[1].next = &nodes[0];
	nodes[2].prev = &nodes[0];

	if (hull.nodes[0].prev == &hull.nodes[1])
		nodes[0].prev = &nodes[1];

	if (hull.nodes[0].next == &hull.nodes[2])
		nodes[0].next = &nodes[2];

	return *this;
}

void Collision::Map::create(const std::vector< std::vector<ivec2> > &lineStrips)
{
	size_t nTotalNodes = 0;

	for (size_t i = 0; i < lineStrips.size(); i++)
		nTotalNodes += std::max(size_t(0), lineStrips[i].size() - 1);

	nodes_.resize(nTotalNodes);

	size_t iStrip = 0;

	for (size_t i = 0; i < lineStrips.size(); i++)
	{
		const std::vector<ivec2> &strip = lineStrips[i];

		if (strip.size() == 0)
			continue;

		for (size_t j = 0; j < strip.size() - 1; j++)
		{
			Collision::Node &node = nodes_[iStrip + j];

			node.line.p1 = fixvec2(fixed(strip[j].x), fixed(strip[j].y));
			node.line.p2 = fixvec2(fixed(strip[j + 1].x), fixed(strip[j + 1].y));
			node.next = j < strip.size() - 2 ? &nodes_[iStrip + j + 1] : 0;
			node.prev = j > 0 ? &nodes_[iStrip + j - 1] : 0;

			node.floor = node.line.p1.x != node.line.p2.x &&
				fpm::fabs(fpm::slope(node.line)) <= fixed(2) &&
				fpm::dot(fixvec2(fixed(0), fixed(-1)), fpm::normal(node.line)) > fixed(0);
		}

		if (strip[0] == strip[strip.size() - 1])
		{
			nodes_[iStrip].prev = &nodes_[iStrip + strip.size() - 2];
			nodes_[iStrip + strip.size() - 2].next = &nodes_[iStrip];
		}

		iStrip += strip.size() - 1;
	}

	#if defined(DEBUG) && DEBUG_COLLISION_DATA
	{
		std::cout << std::endl;
		std::cout << "Collision map data: " << std::endl << std::endl;
		std::stringstream s;

		std::cout << std::setw(4)  << std::left << " ";
		std::cout << std::setw(12) << std::left << "P1";
		std::cout << std::setw(12) << std::left << "P2";
		std::cout << std::setw(16) << std::left << "Normal";
		std::cout << std::setw(5)  << std::left << "Floor";
		std::cout << std::endl;

		for (size_t i = 0; i < nodes_.size(); i++)
		{
			fixvec2 normal = fpm::normal(nodes_[i].line);

			s.str(std::string());
			s.clear();
			s << "#" << i;
			std::cout << std::setw(4) << std::left << s.str();

			s.precision(0);

			s.str(std::string());
			s.clear();
			s << "(" << nodes_[i].line.p1.x << "," << nodes_[i].line.p1.y << ")";
			std::cout << std::setw(12) << std::left << s.str();

			s.str(std::string());
			s.clear();
			s << "(" << nodes_[i].line.p2.x << "," << nodes_[i].line.p2.y << ")";
			std::cout << std::setw(12) << std::left << s.str();

			s.str(std::string());
			s.clear();
			s.precision(3);
			s << "(" << normal.x << "," << normal.y << ")";
			std::cout << std::setw(16) << std::left << s.str();

			s.str(std::string());
			s.clear();
			s << (nodes_[i].floor ? "true" : "false");
			std::cout << std::setw(5) << std::left << s.str();

			std::cout << std::endl;
		}

		std::cout << std::endl;
	}
	#endif
}

const std::vector<const Collision::Node*> &Collision::Map::retrieve(const fixrect &rc) const
{
	cache_.resize(nodes_.size());

	for (size_t i = 0; i < nodes_.size(); i++)
		cache_[i] = &nodes_[i];

	return cache_;
}

Collision::Result Collision::resolve(const Collision::Map &map, const fixvec2 &position, const fixvec2 &dest, const fixrect &bbox)
{
	Result result;
	result.position = dest;
	result.percent = fixed::one;

	fixrect bboxStart = bbox + position;
	fixrect bboxEnd = bbox + dest;
	fixrect bounds = fpm::expand(bboxStart, bboxEnd);

	const std::vector<const Collision::Node*> &nodes = map.retrieve(bounds);

	const fixline pathLine(position, dest);
	const fixvec2 delta = dest - position;
	const fixed epsilon = fixed::from_value(2048); // 1/32
	fixvec2 ndelta; // normalized delta, will be calculated inside loop if necessary

	for (size_t iNode = 0; iNode < nodes.size(); iNode++)
	{
		const Collision::Node *const node = nodes[iNode];

		if (!fpm::intersects(fpm::bounds(node->line), bounds))
			continue;

		Collision::Hull hull = createHull(node, bbox);

		for (int iHullNode = 0; iHullNode < 3; iHullNode++)
		{
			const fixline &hullLine = hull.nodes[iHullNode].line;

			if (hullLine.p1 == hullLine.p2)
				continue;

			if (fpm::dot(fpm::normal(hullLine), delta) > fixed::zero)
				continue;

			fixvec2 intersection;

			if (!fpm::intersection(hullLine, pathLine, &intersection))
				continue;

			if (fpm::fabs(delta.x) < epsilon) intersection.x = position.x;
			if (fpm::fabs(delta.y) < epsilon) intersection.y = position.y;

			fixed percent = fixed::zero;

			if (fpm::fabs(delta.x) > fpm::fabs(delta.y))
				percent = (intersection.x - position.x) / delta.x;
			else
				percent = (intersection.y - position.y) / delta.y;

			if (percent < result.percent)
			{
				result.percent = percent;
				result.node = node;
				result.hull = hull;
				result.iHullNode = iHullNode;
				result.position.x = intersection.x;
				result.position.y = intersection.y;

				// go back a little...

				fixvec2 offset;

				if (delta.x == fixed::zero)
				{
					offset.y = epsilon * fpm::sign(delta.y);
				}
				else if (delta.y == fixed::zero)
				{
					offset.x = epsilon * fpm::sign(delta.x);
				}
				else
				{
					if (ndelta.x == fixed::zero && ndelta.y == fixed::zero)
						ndelta = fpm::normalize(delta);

					offset = ndelta * epsilon;
				}

				result.position -= offset;

				// ...but not too much

				fixvec2 rdelta = result.position - position; // result delta

				if (fpm::sign(rdelta) != fpm::sign(delta))
				{
					result.position = position;
					result.percent = fixed::zero;
					return result;
				}
			}
		}
	}

	return result;
}

Collision::Hull Collision::createHull(const Collision::Node *node, const fixrect &bbox)
{
	Collision::Hull hull;

	hull.nodes[0].line = node->line;
	hull.nodes[0].prev = &hull.nodes[1];
	hull.nodes[0].next = &hull.nodes[2];
	hull.nodes[0].floor = node->floor;
	hull.nodes[1].next = &hull.nodes[0];
	hull.nodes[1].floor = node->floor;
	hull.nodes[2].prev = &hull.nodes[0];
	hull.nodes[2].floor = node->floor;

	const bool prevFloor = node->prev && node->prev->floor;
	const bool nextFloor = node->next && node->next->floor;

	const fixed L = bbox.tl.x;
	const fixed R = bbox.br.x;
	const fixed T = -bbox.tl.y;
	const fixed B = -bbox.br.y;

	fixline *lines[] = {
		&(hull.nodes[0].line),
		&(hull.nodes[1].line),
		&(hull.nodes[2].line)
	};

	fixvec2 &p1 = lines[0]->p1;
	fixvec2 &p2 = lines[0]->p2;

	if (node->floor)
	{
		if (!prevFloor)
		{
			lines[1]->p1 = fixvec2(p1.x + R, p1.y);
			lines[1]->p2 = fixvec2(p1.x    , p1.y);
		}
		else
			hull.nodes[0].prev = node->prev;

		if (!nextFloor)
		{
			lines[2]->p1 = fixvec2(p2.x    , p2.y);
			lines[2]->p2 = fixvec2(p2.x + L, p2.y);
		}
		else
			hull.nodes[0].next = node->next;

		return hull;
	}

	fixvec2 normal = fpm::normal(node->line);

	if (normal.y == fixed::one)
	{
		p1 += fixvec2(L, T);
		p2 += fixvec2(R, T);
		lines[1]->p1 = fixvec2(p1.x, p1.y - bbox.height());
		lines[1]->p2 = fixvec2(p1.x, p1.y);
		lines[2]->p1 = fixvec2(p2.x, p2.y);
		lines[2]->p2 = fixvec2(p2.x, p2.y - bbox.height());
	}
	else if (normal.y == -fixed::one)
	{
		p1 += fixvec2(R, B);
		p2 += fixvec2(L, B);
		lines[1]->p1 = fixvec2(p1.x, p1.y + bbox.height());
		lines[1]->p2 = fixvec2(p1.x, p1.y);
		lines[2]->p1 = fixvec2(p2.x, p2.y);
		lines[2]->p2 = fixvec2(p2.x, p2.y + bbox.height());
		hull.nodes[1].floor = false;
		hull.nodes[2].floor = false;
	}
	else if (normal.x == fixed::one)
	{
		p1 += fixvec2(R, T);
		p2 += fixvec2(R, B);
		lines[1]->p1 = fixvec2(p1.x - bbox.width(), p1.y);
		lines[1]->p2 = fixvec2(p1.x, p1.y);

		if (!nextFloor)
		{
			lines[2]->p1 = fixvec2(p2.x, p2.y);
			lines[2]->p2 = fixvec2(p2.x - bbox.width(), p2.y);
			hull.nodes[2].floor = true;
		}
	}
	else if (normal.x == -fixed::one)
	{
		p1 += fixvec2(L, B);
		p2 += fixvec2(L, T);

		if (!prevFloor)
		{
			lines[1]->p1 = fixvec2(p1.x + bbox.width(), p1.y);
			lines[1]->p2 = fixvec2(p1.x, p1.y);
			hull.nodes[1].floor = true;
		}

		lines[2]->p1 = fixvec2(p2.x, p2.y);
		lines[2]->p2 = fixvec2(p2.x + bbox.width(), p2.y);
	}
	else
	{
		fixed dx = normal.x > fixed::zero ? R : L;
		fixed dy = normal.y > fixed::zero ? T : B;

		p1 += fixvec2(dx, dy);
		p2 += fixvec2(dx, dy);

		if (normal.x > fixed::zero && normal.y > fixed::zero)
		{
			lines[1]->p1 = fixvec2(p1.x - bbox.width(), p1.y);
			lines[1]->p2 = fixvec2(p1.x, p1.y);
			lines[2]->p1 = fixvec2(p2.x, p2.y);
			lines[2]->p2 = fixvec2(p2.x, p2.y - bbox.height());
		}
		else if (normal.x < fixed::zero && normal.y < fixed::zero)
		{
			if (!prevFloor)
			{
				lines[1]->p1 = fixvec2(p1.x + bbox.width(), p1.y);
				lines[1]->p2 = fixvec2(p1.x, p1.y);
				hull.nodes[1].floor = true;
			}

			lines[2]->p1 = fixvec2(p2.x, p2.y);
			lines[2]->p2 = fixvec2(p2.x, p2.y + bbox.height());
		}
		else if (normal.x < fixed::zero && normal.y > fixed::zero)
		{
			lines[1]->p1 = fixvec2(p1.x, p1.y - bbox.height());
			lines[1]->p2 = fixvec2(p1.x, p1.y);
			lines[2]->p1 = fixvec2(p2.x, p2.y);
			lines[2]->p2 = fixvec2(p2.x + bbox.width(), p2.y);
		}
		else
		{
			lines[1]->p1 = fixvec2(p1.x, p1.y + bbox.height());
			lines[1]->p2 = fixvec2(p1.x, p1.y);

			if (!nextFloor)
			{
				lines[2]->p1 = fixvec2(p2.x, p2.y);
				lines[2]->p2 = fixvec2(p2.x - bbox.width(), p2.y);
				hull.nodes[2].floor = true;
			}
		}
	}

	return hull;
}
