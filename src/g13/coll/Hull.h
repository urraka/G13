#pragma once

namespace g13 {
namespace coll {

class Hull
{
public:
	Hull();
	Hull(const Hull &hull);
	Hull(const Segment &segment, const fixrect &bbox);
	Hull& operator =(const Hull &hull);

	Segment segments[3];
};

}} // g13::coll
