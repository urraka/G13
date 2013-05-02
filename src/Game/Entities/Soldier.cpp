#include "Soldier.h"
#include "../Replay.h"

Soldier::Soldier()
{
	physics.input = &input;
	graphics.input = &input;
	graphics.physics = &physics;
}

void Soldier::update(Time dt, Replay *replay)
{
	input.update(replay);
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
