#pragma once

namespace g13 {
namespace coll {

enum Mode
{
	Static,
	Dynamic,
	All
};

class World
{
public:
	World(const fixrect &bounds);
	~World();

	typedef std::vector<fixvec2> Linestrip;

	void create(const std::vector<Linestrip> &linestrip);

	void add(const Entity &entity);
	void clear();

	Result collision(const fixvec2 &a, const fixvec2 &b, const fixrect &bbox, Mode mode) const;

	const fixed &gravity() const;
	const fixrect &bounds() const;

	void gravity(fixed value);

private:
	Grid<Segment> *segmentsGrid_;
	Grid<Entity>  *entitiesGrid_;

	fixrect bounds_;
	fixed gravity_;

	mutable std::vector<int> sharedIndices_;
	mutable std::vector<const Segment*> segments_;
	mutable std::vector<const Entity*> entities_;

	template<typename T> std::vector<const T*> &retrieve(const fixrect &bounds) const;

	template<typename T> inline std::vector<const T*> &cache() const;
	template<typename T> inline const Grid<T> &grid() const;
};

// World specialization

template<> inline const Grid<Segment> &World::grid<Segment>() const
{
	return *segmentsGrid_;
}

template<> inline std::vector<const Segment*> &World::cache<Segment>() const
{
	return segments_;
}

template<> inline const Grid<Entity> &World::grid<Entity>() const
{
	return *entitiesGrid_;
}

template<> inline std::vector<const Entity*> &World::cache<Entity>() const
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

template<> inline fixrect Grid<Entity>::bounds(const Entity &item)
{
	return fpm::expand(item.previus, item.current);
}

template<> inline bool Grid<Entity>::intersects(const Entity &item, const fixrect &bounds)
{
	// TODO: change fpm::expand with something better

	return fpm::intersects(bounds, fpm::expand(item.previus, item.current));
}

}} // g13::coll
