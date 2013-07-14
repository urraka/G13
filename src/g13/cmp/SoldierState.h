#pragma once

#include <g13/math.h>

namespace g13 {
namespace cmp {

struct SoldierState
{
	fixvec2 position;
	fixvec2 velocity;
	bool    flip;
	bool    duck;
	bool    floor;
};

}} // g13::cmp
