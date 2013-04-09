#include <Math/math.h>

#include <iostream>
#include <assert.h>
#include <list>

namespace
{
	struct vertex_t
	{
		vertex_t *prev;
		vertex_t *next;
		size_t index;
		float winding_value;
		bool reflex;
	};

	inline float winding_value(const std::vector<vec2> &polygon, const vertex_t &vertex)
	{
		const vec2 &a = polygon[vertex.prev->index];
		const vec2 &b = polygon[vertex.index];
		const vec2 &c = polygon[vertex.next->index];

		return (b.x - a.x) * (c.y - b.y) - (c.x - b.x) * (b.y - a.y);
	}

	inline bool winding_cw(const vertex_t &vertex)
	{
		return vertex.winding_value < 0;
	}

	inline bool winding_ccw(const vertex_t &vertex)
	{
		return vertex.winding_value > 0;
	}

	inline bool is_ear_tip(const std::vector<vec2> &polygon, const vertex_t &vertex, const std::list<vertex_t*> &reflexVertices)
	{
		if (vertex.reflex)
			return false;

		const vec2 &a = polygon[vertex.prev->index];
		const vec2 &b = polygon[vertex.index];
		const vec2 &c = polygon[vertex.next->index];

		Triangle triangle(a, b, c);
		std::list<vertex_t*>::const_iterator it;

		for (it = reflexVertices.begin(); it != reflexVertices.end(); it++)
		{
			size_t index = (*it)->index;

			if (index == vertex.prev->index || index == vertex.next->index)
				continue;

			if (triangle.contains(polygon[index]))
				return false;
		}

		return true;
	}
}

namespace math
{
	std::vector<Triangle> triangulate(const std::vector<vec2> &polygon)
	{
		const size_t N = polygon.size();
		std::vector<Triangle> triangles;

		if (N <= 2)
			return triangles;

		if (N == 3)
		{
			triangles.push_back(Triangle(polygon[0], polygon[1], polygon[2]));
			return triangles;
		}

		int cwCount = 0;
		int ccwCount = 0;
		std::vector<vertex_t> vertices(N);

		for (size_t i = 1; i <= N; i++)
		{
			size_t index = i % N;
			vertex_t &v = vertices[index];

			v.index = index;
			v.prev = &vertices[(i - 1) % N];
			v.next = &vertices[(i + 1) % N];
			v.winding_value = winding_value(polygon, v);
			v.reflex = false;

			if (v.winding_value > 0.0f)
				ccwCount++;
			else if (v.winding_value < 0.0f)
				cwCount++;
		}

		assert(ccwCount - cwCount != 0);
		bool (*isReflex)(const vertex_t&) = (ccwCount - cwCount > 0) ? winding_cw : winding_ccw;
		std::list<vertex_t*> reflexVertices;

		for (size_t i = 0; i < N; i++)
		{
			if (isReflex(vertices[i]))
			{
				vertices[i].reflex = true;
				reflexVertices.push_back(&vertices[i]);
			}
		}

		size_t skipped = 0;
		size_t iTriangle = 0;
		size_t nVertices = vertices.size();
		vertex_t *current = &vertices[0];
		triangles.resize(N - 2);

		while (nVertices > 3)
		{
			vertex_t *prev = current->prev;
			vertex_t *next = current->next;

			if (is_ear_tip(polygon, *current, reflexVertices))
			{
				triangles[iTriangle].a = polygon[prev->index];
				triangles[iTriangle].b = polygon[current->index];
				triangles[iTriangle].c = polygon[next->index];

				prev->next = next;
				next->prev = prev;

				vertex_t *adjacent[2] = {prev, next};

				for (int i = 0; i < 2; i++)
				{
					if (adjacent[i]->reflex)
					{
						adjacent[i]->winding_value = winding_value(polygon, *adjacent[i]);
						adjacent[i]->reflex = isReflex(*adjacent[i]);

						if (!adjacent[i]->reflex)
							reflexVertices.remove(adjacent[i]);
					}
				}

				if (current->reflex)
					reflexVertices.remove(current);

				iTriangle++;
				nVertices--;
				skipped = 0;
			}
			else if (++skipped > nVertices)
			{
				std::cerr << "Cannot triangulate polygon." << std::endl;
				triangles.clear();
				return triangles;
			}

			current = next;
		}

		triangles[iTriangle].a = polygon[current->prev->index];
		triangles[iTriangle].b = polygon[current->index];
		triangles[iTriangle].c = polygon[current->next->index];

		return triangles;
	}
}
