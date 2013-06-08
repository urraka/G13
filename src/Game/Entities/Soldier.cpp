#include "Soldier.h"
#include "../Replay.h"

namespace ent
{
	Soldier::Soldier()
	{
		physics.input = &input;
		graphics.input = &input;
		graphics.physics = &physics;
	}

	void Soldier::update(Time dt, cmp::SoldierInput *inpt)
	{
		if (inpt != 0)
			input = *inpt;
		else
			input.update();

		physics.update(dt);
		graphics.update(dt);
	}

	void Soldier::reset(fixvec2 pos)
	{
		physics.reset(pos);
		graphics.position.set(math::from_fixed(pos));
	}

	void Soldier::map(const Collision::Map *map)
	{
		physics.map = map;
	}
}
