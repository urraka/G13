#include <iostream>
#include <algorithm>
#include <vector>
#include <g13/coll/collision.h>

namespace g13 {

struct Range
{
	Range() {}
	Range(const fixed &a, const fixed &b) : x(a), lenght(b) {}
	fixed x;
	fixed lenght;
};

static std::vector<fixed> range_positions(const Range &a, const Range &b)
{
	fixed positions[] = {
		a.x - (b.lenght + b.lenght / 2),
		a.x - b.lenght,
		a.x - (b.lenght / 2),
		a.x - b.lenght + a.lenght / 2,
		a.x - (b.lenght - a.lenght) / 2,
		a.x,
		a.x + a.lenght - b.lenght,
		a.x + a.lenght - (a.lenght / 2),
		a.x + a.lenght - (b.lenght / 2),
		a.x + a.lenght,
		a.x + a.lenght + b.lenght / 2
	};

	std::vector<fixed> result(countof(positions));

	for (int i = 0; i < (int)countof(positions); i++)
		result[i] = positions[i];

	return result;
}

static void vec2_out(const fixed &x, const fixed &y)
{
	std::cout << "{x:" << x << ",y:" << y << "}";
}

static void rc_out(const fixrect &rc)
{
	std::cout << "[";
	vec2_out(rc.tl.x, rc.tl.y); std::cout << ", ";
	vec2_out(rc.br.x, rc.tl.y); std::cout << ", ";
	vec2_out(rc.br.x, rc.br.y); std::cout << ", ";
	vec2_out(rc.tl.x, rc.br.y);
	std::cout << "]";
}

static void poly_out(coll::Segment *segments, int n)
{
	std::cout << "[";

	for (int i = 0; i < n - 1; i++)
	{
		vec2_out(segments[i].line.p1.x, segments[i].line.p1.y);
		std::cout << ", ";
	}

	vec2_out(segments[n - 1].line.p1.x, segments[n - 1].line.p1.y);
	std::cout << "]";
}

static void setx(fixrect &rc, const fixed &x)
{
	rc.br.x = x + rc.width();
	rc.tl.x = x;
}

static void sety(fixrect &rc, const fixed &y)
{
	rc.br.y = y + rc.height();
	rc.tl.y = y;
}

static void test(const fixrect &A, fixrect B)
{
	coll::Entity entity;
	coll::Segment segments[8];

	std::vector<fixed> X = range_positions(Range(A.tl.x, A.width()), Range(0, B.width()));
	std::vector<fixed> Y = range_positions(Range(A.tl.y, A.height()), Range(0, B.height()));

	int N = X.size() * Y.size();
	int i = 0;

	for (int y = 0; y < (int)Y.size(); y++)
	{
		sety(B, Y[y]);

		for (int x = 0; x < (int)X.size(); x++)
		{
			setx(B, X[x]);

			entity.previous = A;
			entity.current  = B;

			int n = entity.segments(segments);

			std::cout << "\t{" << std::endl;
			std::cout << "\t\ta: "; rc_out(entity.previous); std::cout << "," << std::endl;
			std::cout << "\t\tb: "; rc_out(entity.current);  std::cout << "," << std::endl;
			std::cout << "\t\tR: "; poly_out(segments, n);   std::cout << std::endl;
			std::cout << "\t}";

			if (i < N - 1)
				std::cout << ",";

			std::cout << std::endl;

			i++;
		}
	}
}

static void perform_tests()
{
	fixrect A(-20, -30, 20, 30);
	fixrect B(-30, -20, 30, 20);

	std::cout << "show([" << std::endl;
	test(A, B);
	std::cout << "," << std::endl;
	test(B, A);
	std::cout << "]);" << std::endl;
}

}
