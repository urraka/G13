#pragma once

#include <g13/math.h>

namespace g13 {
namespace cmp {

class SoldierState
{
public:
	fixvec2  position;
	fixvec2  velocity;
	uint16_t angle;
	bool     rightwards;
	bool     duck;
	bool     floor;
};

}} // g13::cmp
