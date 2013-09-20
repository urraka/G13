#include <vector>
#include <list>

namespace math {
namespace tri {

struct vertex_t
{
	vertex_t *prev;
	vertex_t *next;
	int       index;
	float     winding_value;
	bool      reflex;
};

template<typename vec2_t>
bool triangle_contains(const vec2_t &a, const vec2_t &b, const vec2_t &c, const vec2_t &point)
{
	if ((point.x == a.x && point.y == a.y) ||
		(point.x == b.x && point.y == b.y) ||
		(point.x == c.x && point.y == c.y))
		return false;

	float A = 0.5f * (-b.y * c.x + a.y * (-b.x + c.x) + a.x * (b.y - c.y) + b.x * c.y);
	float sign = A < 0.0f ? -1.0f : 1.0f;

	float s = (a.y * c.x - a.x * c.y + (c.y - a.y) * point.x + (a.x - c.x) * point.y) * sign;
	float t = (a.x * b.y - a.y * b.x + (a.y - b.y) * point.x + (b.x - a.x) * point.y) * sign;

	return s >= 0.0f && t >= 0.0f && (s + t) <= (2.0f * A * sign);
}

template<typename vec2_t>
float winding_value(const std::vector<vec2_t> &polygon, const vertex_t &vertex)
{
	const vec2_t &a = polygon[vertex.prev->index];
	const vec2_t &b = polygon[vertex.index];
	const vec2_t &c = polygon[vertex.next->index];

	return (b.x - a.x) * (c.y - b.y) - (c.x - b.x) * (b.y - a.y);
}

template<typename vec2_t>
bool is_ear_tip(const std::vector<vec2_t> &polygon, const vertex_t &vertex,
	const std::list<vertex_t*> &reflexVertices)
{
	if (vertex.reflex)
		return false;

	const vec2_t &a = polygon[vertex.prev->index];
	const vec2_t &b = polygon[vertex.index];
	const vec2_t &c = polygon[vertex.next->index];

	std::list<vertex_t*>::const_iterator it;

	for (it = reflexVertices.begin(); it != reflexVertices.end(); it++)
	{
		int index = (*it)->index;

		if (index == vertex.prev->index || index == vertex.next->index)
			continue;

		if (triangle_contains(a, b, c, polygon[index]))
			return false;
	}

	return true;
}

template<typename index_t, typename vec2_t>
std::vector<index_t> triangulate(const std::vector<vec2_t> &polygon)
{
	// expected: polygon[N - 1] != polygon[0]

	const int N = polygon.size();
	std::vector<index_t> triangles;

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

	std::vector<vertex_t> vertices(N);

	int iLeftMost = 0;

	#define update_leftmost(i) \
	{ \
		const vec2_t &p  = polygon[i];                                   \
		const vec2_t &lm = polygon[iLeftMost];                           \
		if (p.x < lm.x || (p.x == lm.x && p.y < lm.y)) iLeftMost = i;    \
	}

	#define init_vertex(i, iPrev, iNext) \
		vertices[i].index = i;                                           \
		vertices[i].prev = &vertices[iPrev];                             \
		vertices[i].next = &vertices[iNext];                             \
		vertices[i].prev->index = iPrev;                                 \
		vertices[i].next->index = iNext;                                 \
		vertices[i].winding_value = winding_value(polygon, vertices[i]); \
		vertices[i].reflex = false;

	// initialize vertices

	init_vertex(0    , N - 1, 1);
	init_vertex(N - 1, N - 2, 0);

	update_leftmost(N - 1);

	for (int i = 1; i < N - 1; i++)
	{
		init_vertex(i, i - 1, i + 1);
		update_leftmost(i);
	}

	// check if polygon is ccw

	bool ccw = vertices[iLeftMost].winding_value > 0.0f;

	// initialize list of reflex vertices

	#define is_reflex(v) (ccw ? (v).winding_value <= 0.0f : (v).winding_value >= 0.0f)

	std::list<vertex_t*> reflexVertices;

	for (int i = 0; i < N; i++)
	{
		if (is_reflex(vertices[i]))
		{
			vertices[i].reflex = true;
			reflexVertices.push_back(&vertices[i]);
		}
	}

	// perform triangulation

	int skipped = 0;
	int iTriangle = 0;
	int nVertices = vertices.size();
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

			iTriangle += 3;
			nVertices--;
			skipped = 0;
		}
		else if (++skipped > nVertices)
		{
			triangles.clear();
			return triangles;
		}

		current = next;
	}

	triangles[iTriangle + 0] = current->prev->index;
	triangles[iTriangle + 1] = current->index;
	triangles[iTriangle + 2] = current->next->index;

	#undef update_leftmost
	#undef init_vertex
	#undef is_reflex

	return triangles;
}

} // tri

template<typename index_t, typename vec2_t>
std::vector<index_t> triangulate(const std::vector<vec2_t> &polygon)
{
	return tri::triangulate<index_t, vec2_t>(polygon);
}

} // math
