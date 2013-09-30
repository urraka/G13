#pragma once

namespace g13 {
namespace coll {

class Entity
{
public:
	Entity() : data(0) {}

	void   *data;
	fixrect previus;
	fixrect current;
};

}} // g13::coll
