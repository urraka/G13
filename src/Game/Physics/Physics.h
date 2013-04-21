#pragma once

#include <Math/math.h>

typedef fpm::fixed fixed;
typedef fpm::vec2 fixvec2;
typedef fpm::rect fixrect;
typedef fpm::line fixline;

class Physics
{
public:
	struct CollisionResult
	{
		fixvec2 position;
		fixed percent;
	};

	Physics();
	~Physics();

	CollisionResult collision(fixvec2 position, fixvec2 dest, fixrect bbox);
};
