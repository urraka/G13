#pragma once

#include <g13/math.h>
#include "Segment.h"

namespace g13 {
namespace coll {

class Hull
{
public:
	Hull();
	Hull(const Hull &hull);
	Hull(const Segment &segment, const fixrect &bbox);
	Hull& operator =(const Hull &hull);

	bool owns(const Segment *segment) const;

	Segment segments[3];
};

}} // g13::coll
