#include "ServerPlayer.h"
#include <assert.h>

namespace g13 {
namespace net {

void ServerPlayer::initialize()
{
	Player::initialize();

	peer = Peer(0);
	tick = 0;
	connectTick = 0;
	disconnectCountdown = -1;
	inputTick = 0;

	inputs.clear();

	boundsbufTick = 0;
	boundsbuf.clear();

	pingTick = -1; // Server::onPong() checks this
	pongTick = -1;
}

void ServerPlayer::update(Time dt, int serverTick, coll::World &world)
{
	assert(state == Playing);

	size_t i;

	for (i = 0; i < inputs.size() && inputs[i].tick < serverTick; i++, tick++)
	{
		soldier.update(dt, world, inputs[i].data);

		if (boundsbuf.full())
			boundsbufTick++;

		boundsbuf.push(soldier.physics.bounds() + soldier.physics.position);
	}

	inputs.erase(inputs.begin(), inputs.begin() + i);
}

const coll::Entity *ServerPlayer::entity(int tick)
{
	assert(boundsbuf.size() >= 1);

	int min = boundsbufTick + 1;
	int max = boundsbufTick + boundsbuf.size() - 1;

	if (min <= max)
	{
		tick = glm::clamp<int>(tick, min, max);

		soldier.entity.previous = boundsbuf[tick - boundsbufTick - 1];
		soldier.entity.current  = boundsbuf[tick - boundsbufTick];
	}
	else
	{
		soldier.entity.previous = boundsbuf[0];
		soldier.entity.current  = boundsbuf[0];
	}

	return &soldier.entity;
}

}} // g13::net
