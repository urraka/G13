#pragma once

namespace g13 {
namespace coll {

class Entity
{
public:
	Entity() : data(0), active(true) {}

	void   *data;
	fixrect previous;
	fixrect current;
	bool    active;

	int motionBounds(Segment (&segments)[8]) const;
};

}} // g13::coll
