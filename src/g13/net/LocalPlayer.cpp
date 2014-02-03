#include "LocalPlayer.h"
#include <g13/coll/World.h>
#include <assert.h>

namespace g13 {
namespace net {

void LocalPlayer::initialize()
{
	Player::initialize();

	tick = 0;
	input = cmp::SoldierInput();
}

void LocalPlayer::update(Time dt, coll::World &world)
{
	assert(state == Playing);

	soldier.entity.previous = soldier.physics.bounds() + soldier.physics.position;

	soldier.update(dt, world, input);

	soldier.entity.current = soldier.physics.bounds() + soldier.physics.position;

	world.add(&soldier.entity);

	tick++;
}

void LocalPlayer::updateInput()
{
	input.updateTargetAngle(soldier);
}

}} // g13::net
