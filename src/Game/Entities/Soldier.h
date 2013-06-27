#pragma once

#include "../../Math/math.h"
#include "../../System/Clock.h"
#include "../Collision.h"
#include "../Components/SoldierInput.h"
#include "../Components/SoldierPhysics.h"
#include "../Components/SoldierGraphics.h"

class Replay;

namespace ent
{
	class Soldier
	{
	public:
		Soldier();

		void update(Time dt, cmp::SoldierInput *inpt = 0);
		void reset(fixvec2 pos);
		void map(const Collision::Map *map);

		cmp::SoldierInput input;
		cmp::SoldierPhysics physics;
		cmp::SoldierGraphics graphics;

		struct State
		{
			uint8_t playerId;
			fixvec2 position;
			fixvec2 velocity;
			bool    flip;
			bool    duck;
			bool    floor;
		};
	};
}
