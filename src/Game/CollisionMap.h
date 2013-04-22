#pragma once

#include <vector>
#include <Math/math.h>

typedef fpm::fixed fixed;
typedef fpm::vec2 fixvec2;
typedef fpm::rect fixrect;
typedef fpm::line fixline;

class CollisionMap
{
public:
	struct LineNode
	{
		const LineNode *prev;
		const LineNode *next;
		fixline line;
		bool floor;
	};

	struct Hull
	{
		LineNode nodes[3];
	};

	struct CollisionResult
	{
		fixvec2 position;
		fixed percent;
		Hull hull;
		int iHullNode;
		const LineNode *node;
	};

	CollisionMap();
	~CollisionMap();

	CollisionResult collision(const fixvec2 &position, const fixvec2 &dest, const fixrect &bbox);

private:
	std::vector<LineNode> nodes_;

	Hull createHull(const LineNode *node, const fixrect &bbox);
};
