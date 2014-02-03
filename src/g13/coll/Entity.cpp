#include "Entity.h"
#include "Segment.h"
#include <algorithm>
#include <assert.h>

namespace g13 {
namespace coll {

static int update_segments(Segment *segments, const fixvec2 *linestrip, int count)
{
	for (int i = count - 1; i >= 0; i--)
	{
		segments[i].floor = false;
		segments[i].line.p1 = linestrip[i];
		segments[i].line.p2 = linestrip[(count + i - 1) % count];
		segments[i].prev = &segments[(i + 1) % count];
		segments[i].next = &segments[(count + i - 1) % count];
	}

	return count;
}

int Entity::boundingSegments(Segment (&segments)[8]) const
{
	const fixrect &A = previous;
	const fixrect &B = current;

	// case 1: rects are aligned or self contained

	if ((A.tl.x == B.tl.x && A.br.x == B.br.x) || (A.tl.y == B.tl.y && A.br.y == B.br.y) ||
		fpm::contains(A, B) || fpm::contains(B, A))
	{
		fixrect rc = fpm::expand(A, B);

		const fixvec2 linestrip[] = {
			fixvec2(rc.tl.x, rc.tl.y),
			fixvec2(rc.br.x, rc.tl.y),
			fixvec2(rc.br.x, rc.br.y),
			fixvec2(rc.tl.x, rc.br.y)
		};

		return update_segments(segments, linestrip, countof(linestrip));
	}

	// case 2: remove one vertex from each rect

	const fixvec2 va[] = {
		fixvec2(A.tl.x, A.tl.y),
		fixvec2(A.br.x, A.tl.y),
		fixvec2(A.br.x, A.br.y),
		fixvec2(A.tl.x, A.br.y)
	};

	const fixvec2 vb[] = {
		fixvec2(B.tl.x, B.tl.y),
		fixvec2(B.br.x, B.tl.y),
		fixvec2(B.br.x, B.br.y),
		fixvec2(B.tl.x, B.br.y)
	};

	if (fpm::sign(A.tl.x - B.tl.x) == fpm::sign(A.br.x - B.br.x) &&
		fpm::sign(A.tl.y - B.tl.y) == fpm::sign(A.br.y - B.br.y))
	{
		const fixvec2 &a = A.tl;
		const fixvec2 &b = B.tl;

		// k is the vertex of A that is removed

		int k = a.x < b.x ? (a.y < b.y ? 2 : 1) :
		        a.x > b.x ? (a.y < b.y ? 3 : 0) : -1;

		assert(k != -1);

		fixvec2 linestrip[6];

		for (int i = 0; i < 3; i++)
			linestrip[i] = va[(k + i + 1) % 4];

		for (int i = 0; i < 3; i++)
			linestrip[i + 3] = vb[((k + 2) + i + 1) % 4];

		return update_segments(segments, linestrip, countof(linestrip));
	}

	// case 3: rects that together make shapes like '+' and 'T' (leave shapes like 'L' for case 4)

	if ((A.tl.x != B.tl.x || (A.tl.y != B.tl.y && A.br.y != B.br.y)) &&
		(A.br.x != B.br.x || (A.tl.y != B.tl.y && A.br.y != B.br.y)))
	{
		const fixrect *a = &A, *b = &B;

		if (A.tl.x < B.tl.x)
			std::swap(a, b);

		const fixrect &A = *a, &B = *b;

		if ((B.tl.x < A.tl.x && A.br.x < B.br.x) && (A.tl.y < B.tl.y && B.br.y < A.br.y))
		{
			const fixvec2 linestrip[] = {
				fixvec2(B.tl.x, B.tl.y),
				fixvec2(A.tl.x, A.tl.y),
				fixvec2(A.br.x, A.tl.y),
				fixvec2(B.br.x, B.tl.y),
				fixvec2(B.br.x, B.br.y),
				fixvec2(A.br.x, A.br.y),
				fixvec2(A.tl.x, A.br.y),
				fixvec2(B.tl.x, B.br.y)
			};

			return update_segments(segments, linestrip, countof(linestrip));
		}
	}

	// case 4: one rect W|H contains the other rect W|H

	{
		const fixrect *a = &A, *b = &B;
		const fixvec2 *av = va, *bv = vb;

		if ((B.tl.y <= A.tl.y && A.br.y <= B.br.y && (A.tl.x >= B.tl.x || A.br.x <= B.br.x)) ||
			(B.tl.x <= A.tl.x && A.br.x <= B.br.x && (A.tl.y >= B.tl.y || A.br.y <= B.br.y)))
			std::swap(a, b), std::swap(av, bv);

		const fixrect &A = *a, &B = *b;
		const fixvec2 *va = av, *vb = bv;

		// k is the vertex of A where the polygon starts

		int k = B.br.x > A.br.x ? 2 :
		        B.br.y > A.br.y ? 3 :
		        B.tl.x < A.tl.x ? 0 :
		        B.tl.y < A.tl.y ? 1 : -1;

		assert(k != -1);

		// s is the index of the polygon that should be skipped to merge colinear edges

		int s = A.tl.y == B.tl.y ? (k == 2 ? 3 : k == 0 ? 0 : -1) :
		        A.br.y == B.br.y ? (k == 2 ? 0 : k == 0 ? 3 : -1) :
		        A.tl.x == B.tl.x ? (k == 3 ? 0 : k == 1 ? 3 : -1) :
		        A.br.x == B.br.x ? (k == 3 ? 3 : k == 1 ? 0 : -1) : -1;

		if (s == -1)
		{
			fixvec2 linestrip[6];

			for (int i = 0; i < 4; i++)
				linestrip[i] = va[(k + i) % 4];

			for (int i = 0; i < 2; i++)
				linestrip[i + 4] = vb[((k + 3) + i) % 4];

			return update_segments(segments, linestrip, countof(linestrip));
		}
		else
		{
			fixvec2 linestrip[5];

			for (int i = 0, j = 0; i < 4; i++)
			{
				if (i == s)
					continue;

				linestrip[j] = va[(k + i) % 4];
				j++;
			}

			for (int i = 0; i < 2; i++)
				linestrip[i + 3] = vb[((k + 3) + i) % 4];

			return update_segments(segments, linestrip, countof(linestrip));
		}
	}
}

}} // g13::coll
