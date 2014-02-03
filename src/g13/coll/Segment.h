#pragma once

#include <g13/math.h>

namespace g13 {
namespace coll {

class Segment
{
public:
	Segment() : floor(false), prev(0), next(0) {}

	bool floor;
	fixline line;
	const Segment *prev;
	const Segment *next;
};

}} // g13::coll
