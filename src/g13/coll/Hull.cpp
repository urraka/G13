#include "collision.h"

namespace g13 {
namespace coll {

Hull::Hull() {}

Hull::Hull(const Hull &hull)
{
	*this = hull;
}

Hull& Hull::operator= (const Hull &hull)
{
	segments[0] = hull.segments[0];
	segments[1] = hull.segments[1];
	segments[2] = hull.segments[2];

	// segments[0] is the main segment
	// segments[0] will usually be linked in this way:
	//     segments[1] <- prev <- segments[0] -> next -> segments[2]
	// in some cases segments[0] will be linked to a "real" segment (segment from the collision map),
	// drawing either segments[1] or segments[2] irrelevant

	segments[1].next = &segments[0];
	segments[2].prev = &segments[0];

	if (hull.segments[0].prev == &hull.segments[1])
		segments[0].prev = &segments[1];

	if (hull.segments[0].next == &hull.segments[2])
		segments[0].next = &segments[2];

	return *this;
}

Hull::Hull(const Segment &segment, const fixrect &bbox)
{
	segments[0].line = segment.line;
	segments[0].prev = &segments[1];
	segments[0].next = &segments[2];
	segments[0].floor = segment.floor;

	segments[1].next = &segments[0];
	segments[1].floor = segment.floor;

	segments[2].prev = &segments[0];
	segments[2].floor = segment.floor;

	const bool prevFloor = segment.prev && segment.prev->floor;
	const bool nextFloor = segment.next && segment.next->floor;

	const fixed L = bbox.tl.x;
	const fixed R = bbox.br.x;
	const fixed T = -bbox.tl.y;
	const fixed B = -bbox.br.y;

	fixline *lines[] = {
		&(segments[0].line),
		&(segments[1].line),
		&(segments[2].line)
	};

	fixvec2 &p1 = lines[0]->p1;
	fixvec2 &p2 = lines[0]->p2;

	if (segment.floor)
	{
		if (!prevFloor)
		{
			lines[1]->p1 = fixvec2(p1.x + R, p1.y);
			lines[1]->p2 = fixvec2(p1.x    , p1.y);
		}
		else
			segments[0].prev = segment.prev;

		if (!nextFloor)
		{
			lines[2]->p1 = fixvec2(p2.x    , p2.y);
			lines[2]->p2 = fixvec2(p2.x + L, p2.y);
		}
		else
			segments[0].next = segment.next;

		return;
	}

	fixvec2 normal = fpm::normal(segment.line);

	if (normal.y == 1)
	{
		p1 += fixvec2(L, T);
		p2 += fixvec2(R, T);
		lines[1]->p1 = fixvec2(p1.x, p1.y - bbox.height());
		lines[1]->p2 = fixvec2(p1.x, p1.y);
		lines[2]->p1 = fixvec2(p2.x, p2.y);
		lines[2]->p2 = fixvec2(p2.x, p2.y - bbox.height());
	}
	else if (normal.y == -1)
	{
		p1 += fixvec2(R, B);
		p2 += fixvec2(L, B);
		lines[1]->p1 = fixvec2(p1.x, p1.y + bbox.height());
		lines[1]->p2 = fixvec2(p1.x, p1.y);
		lines[2]->p1 = fixvec2(p2.x, p2.y);
		lines[2]->p2 = fixvec2(p2.x, p2.y + bbox.height());
		segments[1].floor = false;
		segments[2].floor = false;
	}
	else if (normal.x == 1)
	{
		p1 += fixvec2(R, T);
		p2 += fixvec2(R, B);
		lines[1]->p1 = fixvec2(p1.x - bbox.width(), p1.y);
		lines[1]->p2 = fixvec2(p1.x, p1.y);

		if (!nextFloor)
		{
			lines[2]->p1 = fixvec2(p2.x, p2.y);
			lines[2]->p2 = fixvec2(p2.x - bbox.width(), p2.y);
			segments[2].floor = true;
		}
	}
	else if (normal.x == -1)
	{
		p1 += fixvec2(L, B);
		p2 += fixvec2(L, T);

		if (!prevFloor)
		{
			lines[1]->p1 = fixvec2(p1.x + bbox.width(), p1.y);
			lines[1]->p2 = fixvec2(p1.x, p1.y);
			segments[1].floor = true;
		}

		lines[2]->p1 = fixvec2(p2.x, p2.y);
		lines[2]->p2 = fixvec2(p2.x + bbox.width(), p2.y);
	}
	else
	{
		fixed dx = normal.x > 0 ? R : L;
		fixed dy = normal.y > 0 ? T : B;

		p1 += fixvec2(dx, dy);
		p2 += fixvec2(dx, dy);

		if (normal.x > 0 && normal.y > 0)
		{
			lines[1]->p1 = fixvec2(p1.x - bbox.width(), p1.y);
			lines[1]->p2 = fixvec2(p1.x, p1.y);
			lines[2]->p1 = fixvec2(p2.x, p2.y);
			lines[2]->p2 = fixvec2(p2.x, p2.y - bbox.height());
		}
		else if (normal.x < 0 && normal.y < 0)
		{
			if (!prevFloor)
			{
				lines[1]->p1 = fixvec2(p1.x + bbox.width(), p1.y);
				lines[1]->p2 = fixvec2(p1.x, p1.y);
				segments[1].floor = true;
			}

			lines[2]->p1 = fixvec2(p2.x, p2.y);
			lines[2]->p2 = fixvec2(p2.x, p2.y + bbox.height());
		}
		else if (normal.x < 0 && normal.y > 0)
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
				segments[2].floor = true;
			}
		}
	}
}

}} // g13::coll
