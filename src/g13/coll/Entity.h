#pragma once

#include <g13/math.h>

namespace g13 {
namespace coll {

class Segment;

class Entity
{
public:
	Entity() : data(0), active(true) {}

	void   *data;
	fixrect previous;
	fixrect current;
	bool    active;

	int boundingSegments(Segment (&segments)[8]) const;
};

}} // g13::coll
