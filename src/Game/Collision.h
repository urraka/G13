#pragma once

#include <vector>
#include <Math/math.h>

class Collision
{
public:
	class Node
	{
	public:
		Node() : prev(0), next(0), floor(false) {}

		const Node *prev;
		const Node *next;
		fixline line;
		bool floor;
	};

	class Hull
	{
	public:
		Hull();
		Hull(const Hull &hull);
		Hull& operator= (const Hull &hull);

		Collision::Node nodes[3];
	};

	class Result
	{
	public:
		Result() : iHullNode(-1), node(0) {}

		fixvec2 position;
		fixed percent;
		int iHullNode;
		Collision::Hull hull;
		const Collision::Node *node;
	};

	class Map
	{
	public:
		void create(const std::vector< std::vector<ivec2> > &lineStrips);
		const std::vector<const Collision::Node*> &retrieve(const fixrect &rc) const;

	private:
		std::vector<Collision::Node> nodes_;
		mutable std::vector<const Collision::Node*> cache_;
	};

	static Collision::Result resolve(const Collision::Map &map, const fixvec2 &position, const fixvec2 &dest, const fixrect &bbox);
	static Collision::Hull createHull(const Collision::Node *node, const fixrect &bbox);
};
