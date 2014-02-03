#pragma once

#include <g13/g13.h>
#include <g13/math.h>
#include <vector>

#include "Grid.h"
#include "Segment.h"
#include "Entity.h"

namespace g13 {
namespace coll {

enum Mode
{
	Static  = 1,
	Dynamic = 2,
	All     = (Static | Dynamic)
};

typedef const Entity *entity_t;

class World
{
public:
	World();
	~World();

	void load(const Json::Value &data);
	void unload();
	void add(const entity_t &entity);
	void clear();

	Result collision(const fixvec2 &a, const fixvec2 &b, const fixrect &bbox, Mode mode = Static) const;

	const fixed                &gravity()     const { return gravity_;     }
	const fixrect              &bounds()      const { return bounds_;      }
	const std::vector<fixvec2> &spawnpoints() const { return spawnpoints_; }

private:
	typedef std::vector<fixvec2> Linestrip;

	Grid<Segment> *segmentsGrid_;
	Grid<entity_t> *entitiesGrid_;

	fixrect bounds_;
	fixed gravity_;

	std::vector<fixvec2> spawnpoints_;

	mutable std::vector<int> sharedIndices_;
	mutable std::vector<const Segment*> segments_;
	mutable std::vector<const entity_t*> entities_;

	template<typename T> std::vector<const T*> &retrieve(const fixrect &bounds) const;
	template<typename T> inline std::vector<const T*> &cache() const;
	template<typename T> inline const Grid<T> &grid() const;

	void create(const std::vector<Linestrip> &linestrips);
};

// World methods specialization

template<> inline const Grid<Segment> &World::grid<Segment>() const
{
	return *segmentsGrid_;
}

template<> inline std::vector<const Segment*> &World::cache<Segment>() const
{
	return segments_;
}

template<> inline const Grid<entity_t> &World::grid<entity_t>() const
{
	return *entitiesGrid_;
}

template<> inline std::vector<const entity_t*> &World::cache<entity_t>() const
{
	return entities_;
}

// Grid specializations

template<> inline fixrect Grid<Segment>::bounds(const Segment &item)
{
	return fpm::bounds(item.line);
}

template<> inline bool Grid<Segment>::intersects(const Segment &item, const fixrect &bounds)
{
	return fpm::intersects(bounds, item.line);
}

template<> inline fixrect Grid<entity_t>::bounds(const entity_t &item)
{
	return fpm::expand(item->previous, item->current);
}

template<> inline bool Grid<entity_t>::intersects(const entity_t &item, const fixrect &bounds)
{
	// TODO: change fpm::expand with something better

	return fpm::intersects(bounds, fpm::expand(item->previous, item->current));
}

}} // g13::coll
