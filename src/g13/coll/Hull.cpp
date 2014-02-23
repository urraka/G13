#include "Hull.h"

#include <g13/vars.h>
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
	Segment (&S)[3] = segments;

	S[0].line = segment.line;
	S[0].prev = &S[1];
	S[0].next = &S[2];
	S[0].floor = segment.floor;

	S[1].next = &S[0];
	S[1].floor = segment.floor;

	S[2].prev = &S[0];
	S[2].floor = segment.floor;

	const fixed W = bbox.width();
	const fixed H = bbox.height();

	const fixed offsets[] = {
		 bbox.tl.x, // L
		 bbox.br.x, // R
		-bbox.tl.y, // T
		-bbox.br.y  // B
	};

	if (segment.floor)
	{
		if (segment.prev != 0)
		{
			if (segment.prev->floor)
			{
				S[0].prev = segment.prev;
			}
			else
			{
				SegmentType type = segment_type(*segment.prev);

				const ParamsTemplate &pt = params_template[type];

				if (type == TopLeft || type == Top)
				{
					S[1].line.p1.x = S[0].line.p1.x + offsets[0 + pt.offset_l0p2x];
					S[1].line.p1.y = S[0].line.p1.y + offsets[2 + pt.offset_l0p2y];
					S[1].line.p2.x = S[0].line.p1.x;
					S[1].line.p2.y = S[0].line.p1.y;
					S[1].floor = false;
				}
				else if (type == BottomLeft || type == Left)
				{
					S[1].line.p1.x = segment.prev->line.p1.x + offsets[0 + pt.offset_l0p1x];
					S[1].line.p1.y = segment.prev->line.p1.y + offsets[2 + pt.offset_l0p1y];
					S[1].line.p2.x = S[0].line.p1.x;
					S[1].line.p2.y = S[0].line.p1.y;
					S[1].floor = (fpm::fabs(fpm::slope(S[1].line)) <= vars::MaxFloorSlope);
				}
			}
		}
		else
		{
			S[0].prev = 0;
		}

		if (segment.next != 0)
		{
			if (segment.next->floor)
			{
				S[0].next = segment.next;
			}
			else
			{
				SegmentType type = segment_type(*segment.next);

				const ParamsTemplate &pt = params_template[type];

				if (type == TopRight || type == Top)
				{
					S[2].line.p1.x = S[0].line.p2.x;
					S[2].line.p1.y = S[0].line.p2.y;
					S[2].line.p2.x = S[0].line.p2.x + offsets[0 + pt.offset_l0p1x];
					S[2].line.p2.y = S[0].line.p2.y + offsets[2 + pt.offset_l0p1y];

					S[2].floor = false;
				}
				else if (type == BottomRight || type == Right)
				{
					S[2].line.p1.x = S[0].line.p2.x;
					S[2].line.p1.y = S[0].line.p2.y;
					S[2].line.p2.x = segment.next->line.p2.x + offsets[0 + pt.offset_l0p2x];
					S[2].line.p2.y = segment.next->line.p2.y + offsets[2 + pt.offset_l0p2y];

					S[2].floor = (fpm::fabs(fpm::slope(S[2].line)) <= vars::MaxFloorSlope);
				}
			}
		}
		else
		{
			S[0].next = 0;
		}
	}
	else
	{
		SegmentType type = segment_type(segment);

		const ParamsTemplate &pt = params_template[type];

		S[0].line.p1.x += offsets[0 + pt.offset_l0p1x];
		S[0].line.p1.y += offsets[2 + pt.offset_l0p1y];
		S[0].line.p2.x += offsets[0 + pt.offset_l0p2x];
		S[0].line.p2.y += offsets[2 + pt.offset_l0p2y];

		S[1].line.p1.x = S[0].line.p1.x + W * pt.offset_l1p1x;
		S[1].line.p1.y = S[0].line.p1.y + H * pt.offset_l1p1y;
		S[1].line.p2.x = S[0].line.p1.x;
		S[1].line.p2.y = S[0].line.p1.y;
		S[1].floor = pt.floor_l1;

		S[2].line.p1.x = S[0].line.p2.x;
		S[2].line.p1.y = S[0].line.p2.y;
		S[2].line.p2.x = S[0].line.p2.x + W * pt.offset_l2p2x;
		S[2].line.p2.y = S[0].line.p2.y + H * pt.offset_l2p2y;
		S[2].floor = pt.floor_l2;

		if (S[1].floor && segment.prev && segment.prev->floor)
			S[1].line = fixline();

		if (S[2].floor && segment.next && segment.next->floor)
			S[2].line = fixline();

		if (segment.prev && segment.prev->floor)
		{
			if (type == TopRight || type == Top)
			{
				S[1].line.p1.x = segment.line.p1.x;
				S[1].line.p1.y = segment.line.p1.y;
				S[1].line.p2.x = S[0].line.p1.x;
				S[1].line.p2.y = S[0].line.p1.y;
				S[1].floor = false;
			}
			else if (type == BottomRight || type == Right)
			{
				S[0].line.p1 = segment.line.p1;
				S[0].floor = (fpm::fabs(fpm::slope(S[0].line)) <= vars::MaxFloorSlope);
				S[0].prev = segment.prev;

				S[1].line = fixline();
			}
		}

		if (segment.next && segment.next->floor)
		{
			if (type == TopLeft || type == Top)
			{
				S[2].line.p1.x = S[0].line.p2.x;
				S[2].line.p1.y = S[0].line.p2.y;
				S[2].line.p2.x = segment.line.p2.x;
				S[2].line.p2.y = segment.line.p2.y;
				S[2].floor = false;
			}
			else if (type == BottomLeft || type == Left)
			{
				S[0].line.p2 = segment.line.p2;
				S[0].floor = (fpm::fabs(fpm::slope(S[0].line)) <= vars::MaxFloorSlope);
				S[0].next = segment.next;

				S[2].line = fixline();
			}
		}
	}
}

bool Hull::owns(const Segment *segment) const
{
	return segment == &segments[0] || segment == &segments[1] || segment == &segments[2];
}

}} // g13::coll
