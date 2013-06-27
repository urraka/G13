#include "Player.h"
#include "../Map.h"

#include <hlp/assign.hpp>

namespace net
{
	Player::Player()
	{
	}

	void Player::initialize()
	{
		onDisconnect();
	}

	void Player::update(Time dt, uint32_t tick)
	{
		if (true) /* if has inputs (client/server here) */
		{
			// update from input buffer
		}
		else /* if has state (remote client players) */
		{
			// interpolate from state buffer
		}

		lastTick_ = tick;
	}

	void Player::onConnecting()
	{
		state_ = Connecting;
	}

	void Player::onConnect(const char *name)
	{
		state_ = Connected;
		hlp::assign(name_, name);
	}

	void Player::onDisconnect()
	{
		state_ = Disconnected;
	}

	void Player::onJoin(uint32_t tick, const Map *map, const fixvec2 &position)
	{
		state_ = Playing;
		lastTick_ = tick;
		joinTick_ = tick;
		soldier_.reset(position);
		soldier_.map(map->collisionMap());
	}

	void Player::onSoldierState(uint32_t tick, const ent::Soldier::State &soldierState)
	{
		// TODO: push to some buffer
	}

	void Player::onInput(uint32_t tick, const cmp::SoldierInput &input)
	{
		// TODO: push to some buffer
	}

	Player::State Player::state() const
	{
		return state_;
	}

	bool Player::connected() const
	{
		return state_ >= Connected;
	}
}
