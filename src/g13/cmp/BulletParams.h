#pragma once

#include <stdint.h>
#include <g13/math.h>

namespace g13 {
namespace cmp {

class BulletParams
{
public:
	uint8_t playerid;
	fixvec2 position;
	fixed   speed;
	fixed   angle;

	BulletParams() {}

	BulletParams(uint8_t id, const fixvec2 &pos, const fixed &vel, const fixed &ang)
		:	playerid(id),
			position(pos),
			speed(vel),
			angle(ang)
	{
	}
};

}} // g13::cmp
