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
		uint16_t index;
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
			uint16_t index = (*it)->index;

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
	std::vector<uint16_t> triangulate(const std::vector<vec2> &polygon)
	{
		assert(polygon.size() <= 0x10000);

		const size_t N = polygon.size();
		std::vector<uint16_t> triangles;

		if (N <= 2)
			return triangles;

		if (N == 3)
		{
			triangles.resize(3);
			triangles[0] = 0;
			triangles[1] = 1;
			triangles[2] = 2;
			return triangles;
		}

		int cwCount = 0;
		int ccwCount = 0;

		std::vector<vertex_t> vertices(N);

		for (size_t i = 1; i <= N; i++)
		{
			size_t index = i % N;
			vertex_t &vertex = vertices[index];

			vertex.index = static_cast<uint16_t>(index);
			vertex.prev = &vertices[(i - 1) % N];
			vertex.next = &vertices[(i + 1) % N];
			vertex.prev->index = (i - 1) % N;
			vertex.next->index = (i + 1) % N;
			vertex.winding_value = winding_value(polygon, vertex);
			vertex.reflex = false;

			ccwCount += vertex.winding_value > 0.0f;
			cwCount += vertex.winding_value < 0.0f;
		}

		assert(ccwCount - cwCount != 0);

		const float mult = (ccwCount - cwCount > 0) ? 1.0f : -1.0f;
		#define is_reflex(v) ((v).winding_value * mult <= 0.0f)

		std::list<vertex_t*> reflexVertices;

		for (size_t i = 0; i < N; i++)
		{
			if (is_reflex(vertices[i]))
			{
				vertices[i].reflex = true;
				reflexVertices.push_back(&vertices[i]);
			}
		}

		size_t skipped = 0;
		size_t iTriangle = 0;
		size_t nVertices = vertices.size();
		vertex_t *current = &vertices[0];
		triangles.resize(3 * (N - 2));

		while (nVertices > 3)
		{
			vertex_t *prev = current->prev;
			vertex_t *next = current->next;

			if (is_ear_tip(polygon, *current, reflexVertices))
			{
				triangles[iTriangle + 0] = prev->index;
				triangles[iTriangle + 1] = current->index;
				triangles[iTriangle + 2] = next->index;

				prev->next = next;
				next->prev = prev;

				vertex_t *adjacent[2] = {prev, next};

				for (int i = 0; i < 2; i++)
				{
					if (adjacent[i]->reflex)
					{
						adjacent[i]->winding_value = winding_value(polygon, *adjacent[i]);
						adjacent[i]->reflex = is_reflex(*adjacent[i]);

						if (!adjacent[i]->reflex)
							reflexVertices.remove(adjacent[i]);
					}
				}

				if (current->reflex)
					reflexVertices.remove(current);

				iTriangle += 3;
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

		triangles[iTriangle + 0] = current->prev->index;
		triangles[iTriangle + 1] = current->index;
		triangles[iTriangle + 2] = current->next->index;

		return triangles;

		#undef is_reflex
	}
}
