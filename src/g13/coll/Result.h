#pragma once

namespace g13 {
namespace coll {

class Result
{
public:
	Result() : segment(0), index(-1) {}

	const Segment *segment;

	fixvec2 position;
	fixed   percent;
	Hull    hull;
	int     index;
};

}} // g13::coll
