#include <g13/g13.h>
#include "collision.h"
#include <map>

namespace g13 {
namespace coll {

typedef std::map<const Segment*, Grid<Segment>::Location> locationmap_t;

static void map_pointers(Grid<Segment> &grid, Segment &segment, const locationmap_t &map);
static void check_collision(const Segment &seg, const fixline &path, const fixrect &bbox, Result &res);
static const fixed epsilon = fixed::from_value(2048); // 0,03125 = 1 / 32

template std::vector<const Segment*> &World::retrieve<Segment>(const fixrect &bounds) const;
template std::vector<const Entity*> &World::retrieve<Entity>(const fixrect &bounds) const;

// World

World::World(const fixrect &bounds)
	:	segmentsGrid_(0),
		entitiesGrid_(0),
		bounds_(bounds),
		gravity_(1470)
{
	const fixvec2 cellsize(200);

	const int cols = fpm::ceil(bounds.width()  / cellsize.x).to_int();
	const int rows = fpm::ceil(bounds.height() / cellsize.y).to_int();

	segmentsGrid_ = new Grid<Segment>(cols, rows, bounds);
	entitiesGrid_ = new Grid<Entity >(cols, rows, bounds);
}

World::~World()
{
	delete segmentsGrid_;
	delete entitiesGrid_;
}

void World::create(const std::vector<Linestrip> &linestrips)
{
	typedef std::vector<Segment> Segments;

	std::vector<Segments> segmentlists(linestrips.size());

	locationmap_t map;

	for (int i = 0; i < (int)segmentlists.size(); i++)
	{
		Segments &segments = segmentlists[i];

		const Linestrip &linestrip = linestrips[i];

		if (linestrip.size() < 2)
			continue;

		const int N = linestrip.size() - 1;

		segments.resize(N);

		for (int j = 0; j < N; j++)
		{
			Segment &segment = segments[j];

			segment.line.p1 = linestrip[j + 0];
			segment.line.p2 = linestrip[j + 1];

			segment.prev = &segments[(N + j - 1) % N];
			segment.next = &segments[(N + j + 1) % N];

			segment.floor = is_floor(segment.line);

			map[&segment] = segmentsGrid_->push(segment);
		}
	}

	// map prev/next pointers of segments in the grid to new memory location

	for (int i = 0; i < (int)segmentsGrid_->cells().size(); i++)
	{
		Grid<Segment>::Cell &cell = segmentsGrid_->cells()[i];

		for (int j = 0; j < (int)cell.items.size(); j++)
			map_pointers(*segmentsGrid_, cell.items[j], map);
	}

	for (int i = 0; i < (int)segmentsGrid_->sharedItems().size(); i++)
		map_pointers(*segmentsGrid_, segmentsGrid_->sharedItems()[i], map);
}

void World::add(const Entity &entity)
{
	entitiesGrid_->push(entity);
}

void World::clear()
{
	entitiesGrid_->clear();
}

Result World::collision(const fixvec2 &a, const fixvec2 &b, const fixrect &bbox, Mode mode) const
{
	Result result;
	result.position = b;
	result.percent = 1;

	const fixvec2 delta = fpm::epsilon_check(b - a, epsilon);

	if (delta.x == 0 && delta.y == 0)
	{
		result.position = a;
		return result;
	}

	const fixline path   = fixline(a, b);
	const fixrect bounds = fpm::expand(bbox + a, bbox + b);

	if (mode & Static)
	{
		std::vector<const Segment*> &segments = retrieve<Segment>(bounds);

		for (int i = 0; i < (int)segments.size(); i++)
		{
			check_collision(*segments[i], path, bbox, result);

			if (result.percent == 0)
				break;
		}

		// the next is some kind of hack to prevent going through walls
		if (result.position != a)
		{
			fixrect rc = fixrect(
				result.position - fixvec2(epsilon),
				result.position + fixvec2(epsilon)
			);

			for (int i = 0; i < (int)segments.size(); i++)
			{
				const Segment &segment = *segments[i];

				if (!fpm::intersects(fpm::bounds(segment.line), rc))
					continue;

				Hull hull(segment, bbox);

				for (int j = 0; j < 3; j++)
				{
					const fixline &hullLine = hull.segments[j].line;

					if (hullLine.p1 == hullLine.p2)
						continue;

					fixvec2 normal = fpm::normal(hullLine);

					if (fpm::dot(normal, delta) > 0)
						continue;

					fixvec2 point;
					fixline line(result.position, result.position - normal * epsilon);

					if (fpm::intersection(hullLine, line, &point))
					{
						fixvec2 offset = line.p2 - point;
						result.position -= offset;
						rc -= offset;
					}
				}
			}
		}
	}

	if (mode & Dynamic && result.position != a)
	{
		std::vector<const Entity*> &entities = retrieve<Entity>(bounds);

		Segment segments[8];

		for (int i = 0; i < (int)entities.size(); i++)
		{
			int n = entities[i]->motionBounds(segments);

			for (int j = 0; j < n; j++)
			{
				check_collision(segments[i], path, bbox, result);
			}
		}
	}

	return result;
}

template<typename T> std::vector<const T*> &World::retrieve(const fixrect &bounds) const
{
	const Grid<T> &grid = this->grid<T>();
	std::vector<const T*> &cache = this->cache<T>();

	const fixed &w = grid.cellWidth();
	const fixed &h = grid.cellHeight();

	int cols = grid.cols();
	int rows = grid.rows();

	const fixrect rc = bounds - bounds_.tl;

	int x0 = std::max(   0, fpm::floor(rc.tl.x / w).to_int());
	int y0 = std::max(   0, fpm::floor(rc.tl.y / h).to_int());
	int x1 = std::min(cols, fpm::ceil (rc.br.x / w).to_int());
	int y1 = std::min(rows, fpm::ceil (rc.br.y / h).to_int());

	cache.clear();
	sharedIndices_.clear();

	for (int x = x0; x < x1; x++)
	{
		for (int y = y0; y < y1; y++)
		{
			const typename Grid<T>::Cell &cell = grid.cell(x, y);

			for (int i = 0; i < (int)cell.items.size(); i++)
				cache.push_back(&cell.items[i]);

			int top = sharedIndices_.size();

			for (int i = 0; i < (int)cell.shared.size(); i++)
			{
				bool exists = false;

				for (int j = 0; j < top; j++)
				{
					if (cell.shared[i] == sharedIndices_[j])
					{
						exists = true;
						break;
					}
				}

				if (!exists)
					sharedIndices_.push_back(cell.shared[i]);
			}
		}
	}

	const std::vector<T> &sharedItems = grid.sharedItems();

	for (int i = 0; i < (int)sharedIndices_.size(); i++)
		cache.push_back(&sharedItems[sharedIndices_[i]]);

	return cache;
}

void map_pointers(Grid<Segment> &grid, Segment &segment, const locationmap_t &map)
{
	Grid<Segment>::Location loc;

	loc = map.find(segment.prev)->second;

	if (loc.cell != 0)
		segment.prev = &(loc.cell->items[loc.index]);
	else if (loc.index >= -1)
		segment.prev = &(grid.sharedItems()[loc.index]);
	else
		segment.prev = 0;

	loc = map.find(segment.next)->second;

	if (loc.cell != 0)
		segment.next = &(loc.cell->items[loc.index]);
	else if (loc.index >= -1)
		segment.next = &(grid.sharedItems()[loc.index]);
	else
		segment.next = 0;
}

void check_collision(const Segment &segment, const fixline &path, const fixrect &bbox, Result &result)
{
	const fixvec2 &a    = path.p1;
	const fixvec2 &b    = path.p2;
	const fixvec2 delta = fpm::epsilon_check(b - a, epsilon);

	if (!fpm::intersects(fpm::bounds(segment.line), fpm::expand(bbox + a, bbox + b)))
		return;

	Hull hull(segment, bbox);

	for (int i = 0; i < 3; i++)
	{
		const fixline &line = hull.segments[i].line;

		if (line.p1 == line.p2)
			continue;

		if (fpm::dot(fpm::normal(line), delta) > 0)
			continue;

		fixvec2 intersection;

		if (!fpm::intersection(line, path, &intersection))
			continue;

		fixvec2 idelta = intersection - a;

		if (delta.x == 0 || fpm::sign(idelta.x) != fpm::sign(delta.x) || fpm::fabs(idelta.x) < epsilon)
			intersection.x = a.x;

		if (delta.y == 0 || fpm::sign(idelta.y) != fpm::sign(delta.y) || fpm::fabs(idelta.y) < epsilon)
			intersection.y = a.y;

		fixed percent = 0;

		if (fpm::fabs(delta.x) > fpm::fabs(delta.y))
			percent = (intersection.x - a.x) / delta.x;
		else
			percent = (intersection.y - a.y) / delta.y;

		assert(percent >= 0 && percent <= 1 && percent != fixed::overflow);

		if (percent < result.percent)
		{
			result.percent = percent;
			result.segment = &segment;
			result.hull = hull;
			result.index = i;

			// go back a little..
			result.position = intersection - fpm::normalize(delta) * fixed::half;

			#ifdef DEBUG
				if (!fpm::intersection(line, fixline(result.position, b), &intersection))
					debug_log("Warning: result.position -> dest doesn't intersect.");
			#endif

			// ...but not too much

			fixvec2 rdelta = result.position - a; // result delta

			if (fpm::sign(rdelta) != fpm::sign(delta))
			{
				result.position = a;
				result.percent = 0;
				return;
			}
		}
	}

	return;
}

}} // g13::coll
