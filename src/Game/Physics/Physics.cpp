#include <Game/Physics/Physics.h>

Physics::Physics() {}
Physics::~Physics() {}

Physics::CollisionResult Physics::collision(const fixvec2 &position, const fixvec2 &dest, const fixrect &bbox)
{
	CollisionResult result = {};
	result.position = position;
	result.percent = fixed::one;

	fixrect bboxStart = bbox + position;
	fixrect bboxEnd = bbox + dest;
	fixrect bounds = fpm::expand(bboxStart, bboxEnd);

	// TODO: retrieve lines from quadtree using bounds

	const fixline pathLine(position, dest);
	const fixvec2 delta = dest - position;

	for (size_t iNode = 0; iNode < nodes_.size(); iNode++)
	{
		const LineNode *const node = &nodes_[iNode];
		const fixline &line = node->line;

		if (!fpm::intersects(fpm::bounds(line), bounds))
			continue;

		Hull hull = createHull(node, bbox);

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
				result.position.x = position.x + (dest.x - position.x) * percent;
				result.position.y = position.y + (dest.y - position.y) * percent;
			}
		}
	}

	return result;
}

Physics::Hull Physics::createHull(const LineNode *node, const fixrect &bbox)
{
	Hull hull = {};

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

	fixline *line[] = {
		&(hull.nodes[0].line),
		&(hull.nodes[1].line),
		&(hull.nodes[2].line)
	};

	fixvec2 &p1 = line[0]->p1;
	fixvec2 &p2 = line[0]->p2;

	if (node->floor)
	{
		if (!prevFloor)
		{
			line[1]->p1 = fixvec2(p1.x + R, p1.y);
			line[1]->p2 = fixvec2(p1.x    , p1.y);
		}
		else
			hull.nodes[0].prev = node->prev;

		if (!nextFloor)
		{
			line[2]->p1 = fixvec2(p2.x    , p2.y);
			line[2]->p2 = fixvec2(p2.x + L, p2.y);
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
		line[1]->p1 = fixvec2(p1.x, p1.y - bbox.height());
		line[1]->p2 = fixvec2(p1.x, p1.y);
		line[2]->p1 = fixvec2(p2.x, p2.y);
		line[2]->p2 = fixvec2(p2.x, p2.y - bbox.height());
	}
	else if (normal.y == -fixed::one)
	{
		p1 += fixvec2(R, B);
		p2 += fixvec2(L, B);
		line[1]->p1 = fixvec2(p1.x, p1.y + bbox.height());
		line[1]->p2 = fixvec2(p1.x, p1.y);
		line[2]->p1 = fixvec2(p2.x, p2.y);
		line[2]->p2 = fixvec2(p2.x, p2.y + bbox.height());
		hull.nodes[1].floor = false;
		hull.nodes[2].floor = false;
	}
	else if (normal.x == fixed::one)
	{
		p1 += fixvec2(R, T);
		p2 += fixvec2(R, B);
		line[1]->p1 = fixvec2(p1.x - bbox.width(), p1.y);
		line[1]->p2 = fixvec2(p1.x, p1.y);

		if (!nextFloor)
		{
			line[2]->p1 = fixvec2(p2.x, p2.y);
			line[2]->p2 = fixvec2(p2.x - bbox.width(), p2.y);
			hull.nodes[2].floor = true;
		}
	}
	else if (normal.x == -fixed::one)
	{
		p1 += fixvec2(L, B);
		p2 += fixvec2(L, T);

		if (!prevFloor)
		{
			line[1]->p1 = fixvec2(p1.x + bbox.width(), p1.y);
			line[1]->p2 = fixvec2(p1.x, p1.y);
			hull.nodes[1].floor = true;
		}

		line[2]->p1 = fixvec2(p2.x, p2.y);
		line[2]->p2 = fixvec2(p2.x + bbox.width(), p2.y);
	}
	else
	{
		fixed dx = normal.x > fixed::zero ? R : L;
		fixed dy = normal.y > fixed::zero ? T : B;

		p1 += fixvec2(dx, dy);
		p2 += fixvec2(dx, dy);

		if (normal.x > fixed::zero && normal.y > fixed::zero)
		{
			line[1]->p1 = fixvec2(p1.x - bbox.width(), p1.y);
			line[1]->p2 = fixvec2(p1.x, p1.y);
			line[2]->p1 = fixvec2(p2.x, p2.y);
			line[2]->p2 = fixvec2(p2.x, p2.y - bbox.height());
		}
		else if (normal.x < fixed::zero && normal.y < fixed::zero)
		{
			if (!prevFloor)
			{
				line[1]->p1 = fixvec2(p1.x + bbox.width(), p1.y);
				line[1]->p2 = fixvec2(p1.x, p1.y);
				hull.nodes[1].floor = true;
			}

			line[2]->p1 = fixvec2(p2.x, p2.y);
			line[2]->p2 = fixvec2(p2.x, p2.y + bbox.height());
		}
		else if (normal.x < fixed::zero && normal.y > fixed::zero)
		{
			line[1]->p1 = fixvec2(p1.x, p1.y - bbox.height());
			line[1]->p2 = fixvec2(p1.x, p1.y);
			line[2]->p1 = fixvec2(p2.x, p2.y);
			line[2]->p2 = fixvec2(p2.x + bbox.width(), p2.y);
		}
		else
		{
			line[1]->p1 = fixvec2(p1.x, p1.y + bbox.height());
			line[1]->p2 = fixvec2(p1.x, p1.y);

			if (!nextFloor)
			{
				line[2]->p1 = fixvec2(p2.x, p2.y);
				line[2]->p2 = fixvec2(p2.x - bbox.width(), p2.y);
				hull.nodes[2].floor = true;
			}
		}
	}

	return hull;
}
