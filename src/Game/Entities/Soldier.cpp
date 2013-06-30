#include "Soldier.h"
#include "../Replay.h"

namespace ent
{
	Soldier::Soldier()
	{
		physics.input = &input;
		state_.flip = false;
	}

	void Soldier::update(Time dt, cmp::SoldierInput *inpt)
	{
		if (inpt != 0)
			input = *inpt;
		else
			input.update();

		physics.update(dt);
		graphics.update(dt, state());
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

	cmp::SoldierState Soldier::state()
	{
		state_.position = physics.position;
		state_.velocity = physics.velocity;
		state_.duck  = physics.ducking();
		state_.floor = physics.floor();

		if (input.right) state_.flip = true;
		if (input.left) state_.flip = false;

		return state_;
	}
}
