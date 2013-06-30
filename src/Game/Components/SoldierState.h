#pragma once

namespace cmp
{
	struct SoldierState
	{
		fixvec2 position;
		fixvec2 velocity;
		bool    flip;
		bool    duck;
		bool    floor;
	};
}
