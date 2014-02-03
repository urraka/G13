#pragma once

#include <g13/math.h>
#include "Hull.h"

namespace g13 {
namespace coll {

class Segment;
class Entity;

class Result
{
public:
	Result() : segment(0), entity(0), index(-1) {}

	const Segment *segment;
	const Entity  *entity;

	fixvec2 position;
	fixed   percent;
	Hull    hull;
	int     index;

	bool collided() const { return segment != 0 || entity != 0; }

	operator const bool() const { return collided(); }
};

}} // g13::coll
