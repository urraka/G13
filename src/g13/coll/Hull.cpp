#include "Hull.h"
#include <assert.h>

namespace g13 {
namespace coll {

// Segment types:
//
//                Top
//              ________
//     TopLeft /        \ TopRight
//            /          \               note: prev/next order is clockwise
//           |            |
//      Left |      o     | Right
//           |     /|\    |
//            \    / \   /
//  BottomLeft \________/ BottomRight
//               Bottom

enum SegmentType
{
	InvalidType = -1,
	Top = 0, Bottom, Left, Right,
	TopLeft, BottomRight, TopRight, BottomLeft
};

static SegmentType segment_type(const Segment &segment)
{
	fixvec2 normal = fpm::normal(segment.line);

	SegmentType type = normal.y ==  1               ? Top         :
	                   normal.y == -1               ? Bottom      :
	                   normal.x ==  1               ? Left        :
	                   normal.x == -1               ? Right       :
	                   normal.x > 0 && normal.y > 0 ? TopLeft     :
	                   normal.x < 0 && normal.y < 0 ? BottomRight :
	                   normal.x < 0 && normal.y > 0 ? TopRight    :
	                   normal.x > 0 && normal.y < 0 ? BottomLeft  : InvalidType;

	assert(type != InvalidType);

	return type;
}

// some static data to avoid some branches

struct ParamsTemplate
{
	unsigned offset_l0p1x : 1; // indices for L, R or T, B (for x and y respectively)
	unsigned offset_l0p1y : 1;
	unsigned offset_l0p2x : 1;
	unsigned offset_l0p2y : 1;
	signed   offset_l1p1x : 2; // multipliers for W, H
	signed   offset_l1p1y : 2;
	signed   offset_l2p2x : 2;
	signed   offset_l2p2y : 2;
	unsigned floor_l1     : 1; // booleans
	unsigned floor_l2     : 1;
};

static const ParamsTemplate params_template[] = {
//    l0p1   l0p2    l1p1     l2p2   fl1  fl2
	{ 0, 0,  1, 0,   0, -1,   0, -1,  0,   0 }, // Top
	{ 1, 1,  0, 1,   0,  1,   0,  1,  0,   0 }, // Bottom
	{ 1, 0,  1, 1,  -1,  0,  -1,  0,  0,   1 }, // Left
	{ 0, 1,  0, 0,   1,  0,   1,  0,  1,   0 }, // Right
	{ 1, 0,  1, 0,  -1,  0,   0, -1,  0,   0 }, // TopLeft
	{ 0, 1,  0, 1,   1,  0,   0,  1,  1,   0 }, // BottomRight
	{ 0, 0,  0, 0,   0, -1,   1,  0,  0,   0 }, // TopRight
	{ 1, 1,  1, 1,   0,  1,  -1,  0,  0,   1 }  // BottomLeft
};

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

	const fixed &L = bbox.tl.x;
	const fixed &R = bbox.br.x;
	const fixed &T = -bbox.tl.y;
	const fixed &B = -bbox.br.y;
	const fixed  W = bbox.width();
	const fixed  H = bbox.height();

	const fixed values[] = {L, R, T, B};

	fixline *lines[] = {
		&(segments[0].line),
		&(segments[1].line),
		&(segments[2].line)
	};

	fixvec2 &p1 = lines[0]->p1;
	fixvec2 &p2 = lines[0]->p2;

	if (segment.floor)
	{
		/*if (!prevFloor)
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
			segments[0].next = segment.next;*/

		// ----

		if (segment.prev != 0)
		{
			if (segment.prev->floor)
			{
				segments[0].prev = segment.prev;
			}
			else
			{
				SegmentType type = segment_type(*segment.prev);

				const ParamsTemplate &pt = params_template[type];

				if (type == TopLeft || type == Top)
				{
					lines[1]->p1.x = p1.x + values[0 + pt.offset_l0p2x];
					lines[1]->p1.y = p1.y + values[2 + pt.offset_l0p2y];
					lines[1]->p2.x = p1.x;
					lines[1]->p2.y = p1.y;

					segments[1].floor = false;
				}
				else if (type == BottomLeft || type == Left)
				{
					lines[1]->p1.x = segment.prev->line.p1.x + values[0 + pt.offset_l0p1x];
					lines[1]->p1.y = segment.prev->line.p1.y + values[2 + pt.offset_l0p1y];
					lines[1]->p2.x = p1.x;
					lines[1]->p2.y = p1.y;

					// segments[1].floor = false;
					segments[1].floor = (fpm::fabs(fpm::slope(*lines[1])) <= 2);
				}
			}
		}
		else
		{
			segments[0].prev = 0;
		}

		if (segment.next != 0)
		{
			if (segment.next->floor)
			{
				segments[0].next = segment.next;
			}
			else
			{
				SegmentType type = segment_type(*segment.next);

				const ParamsTemplate &pt = params_template[type];

				if (type == TopRight || type == Top)
				{
					lines[2]->p1.x = p2.x;
					lines[2]->p1.y = p2.y;
					lines[2]->p2.x = p2.x + values[0 + pt.offset_l0p1x];
					lines[2]->p2.y = p2.y + values[2 + pt.offset_l0p1y];

					segments[2].floor = false;
				}
				else if (type == BottomRight || type == Right)
				{
					lines[2]->p1.x = p2.x;
					lines[2]->p1.y = p2.y;
					lines[2]->p2.x = segment.next->line.p2.x + values[0 + pt.offset_l0p2x];
					lines[2]->p2.y = segment.next->line.p2.y + values[2 + pt.offset_l0p2y];

					// segments[2].floor = false;
					segments[2].floor = (fpm::fabs(fpm::slope(*lines[2])) <= 2);
				}
			}
		}
		else
		{
			segments[0].next = 0;
		}
	}
	else
	{
		SegmentType type = segment_type(segment);

		const ParamsTemplate &pt = params_template[type];

		p1.x += values[0 + pt.offset_l0p1x];
		p1.y += values[2 + pt.offset_l0p1y];
		p2.x += values[0 + pt.offset_l0p2x];
		p2.y += values[2 + pt.offset_l0p2y];

		lines[1]->p1.x = p1.x + W * pt.offset_l1p1x;
		lines[1]->p1.y = p1.y + H * pt.offset_l1p1y;
		lines[1]->p2.x = p1.x;
		lines[1]->p2.y = p1.y;

		lines[2]->p1.x = p2.x;
		lines[2]->p1.y = p2.y;
		lines[2]->p2.x = p2.x + W * pt.offset_l2p2x;
		lines[2]->p2.y = p2.y + H * pt.offset_l2p2y;

		segments[1].floor = pt.floor_l1;
		segments[2].floor = pt.floor_l2;

		if (segments[1].floor && prevFloor)
		{
			lines[1]->p1.x = lines[1]->p1.y = 0;
			lines[1]->p2.x = lines[1]->p2.y = 0;
		}

		if (segments[2].floor && nextFloor)
		{
			lines[2]->p1.x = lines[2]->p1.y = 0;
			lines[2]->p2.x = lines[2]->p2.y = 0;
		}

		if (prevFloor)
		{
			if (type == TopRight || type == Top)
			{
				lines[1]->p1.x = segment.line.p1.x;
				lines[1]->p1.y = segment.line.p1.y;
				lines[1]->p2.x = p1.x;
				lines[1]->p2.y = p1.y;

				segments[1].floor = false;
			}
			else if (type == BottomRight || type == Right)
			{
				// TODO: this should modify lines[0]

				lines[1]->p1.x = segment.line.p1.x;
				lines[1]->p1.y = segment.line.p1.y;
				lines[1]->p2.x = p2.x;
				lines[1]->p2.y = p2.y;

				// segments[1].floor = false;
				segments[1].floor = (fpm::fabs(fpm::slope(*lines[1])) <= 2);

				p1 = p2 = fixvec2(0);
			}
		}

		if (nextFloor)
		{
			if (type == TopLeft || type == Top)
			{
				lines[2]->p1.x = p2.x;
				lines[2]->p1.y = p2.y;
				lines[2]->p2.x = segment.line.p2.x;
				lines[2]->p2.y = segment.line.p2.y;

				segments[2].floor = false;
			}
			else if (type == BottomLeft || type == Left)
			{
				 // TODO: this should modify lines[0]

				lines[2]->p1.x = p1.x;
				lines[2]->p1.y = p1.y;
				lines[2]->p2.x = segment.line.p2.x;
				lines[2]->p2.y = segment.line.p2.y;

				// segments[2].floor = false;
				segments[2].floor = (fpm::fabs(fpm::slope(*lines[2])) <= 2);

				p1 = p2 = fixvec2(0);
			}
		}
	}
}

bool Hull::owns(const Segment *segment) const
{
	return segment == &segments[0] || segment == &segments[1] || segment == &segments[2];
}

}} // g13::coll
