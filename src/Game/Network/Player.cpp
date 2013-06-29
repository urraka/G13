#include "Player.h"
#include "../Map.h"

#include <hlp/assign.hpp>

namespace net
{
	Player::Player()
		:	mode_(Server),
			stateBase_(0),
			stateTick_(0),
			peer_(0)
	{
		hlp::assign(name_, "");
	}

	void Player::initialize()
	{
		onDisconnect();
	}

	void Player::update(Time dt, uint32_t tick)
	{
		switch (mode_)
		{
			case Local:
			{
				// TODO: add input to a buffer to correct position with a replay
				soldier_.update(dt, &soldier_.input);
			}
			break;

			case Remote:
			{
				// interpolate/extrapolate
			}
			break;

			case Server:
			{
				if (state_ == Connecting)
				{
					connectTimeout_ += dt;

					if (connectTimeout_ >= Clock::milliseconds(5000))
					{
						enet_peer_reset(peer_);
						onDisconnect();
					}
				}
				else
				{
					soldier_.update(dt, &soldier_.input);
				}
			}
			break;
		}
	}

	void Player::onConnecting(ENetPeer *peer)
	{
		state_ = Connecting;
		peer_ = peer;
		connectTimeout_ = 0;
	}

	void Player::onConnect(const char *name)
	{
		state_ = Connected;
		hlp::assign(name_, name);
	}

	void Player::onDisconnect()
	{
		state_ = Disconnected;
		peer_ = 0;
	}

	void Player::onJoin(uint32_t tick, const Map *map, const fixvec2 &position)
	{
		state_ = Playing;
		lastInputTick_ = tick;
		joinTick_ = tick;
		soldier_.reset(position);
		soldier_.map(map->collisionMap());
	}

	void Player::onSoldierState(uint32_t tick, const ent::Soldier::State &soldierState)
	{
		if (tick < stateTick_)
			return;

		const int size = int(sizeof stateBuffer_ / sizeof stateBuffer_[0]);

		int offset = tick - stateTick_;

		if (offset >= size)
		{
			for (int i = 0; i < offset - size && i < size; i++)
				stateBuffer_[(stateBase_ + i) % size].received = false;

			stateBase_ = (stateBase_ + offset - size + 1) % size;
			stateTick_ = tick - size + 1;
		}

		int index = offset % size;
		stateBuffer_[index].received = true;
		stateBuffer_[index].state = soldierState;
	}

	void Player::onInput(uint32_t tick, const cmp::SoldierInput &input)
	{
		if (tick > lastInputTick_)
		{
			soldier_.input = input;
			lastInputTick_ = tick;
		}
	}

	void Player::mode(Mode m)
	{
		mode_ = m;
	}

	Player::Mode Player::mode() const
	{
		return mode_;
	}

	Player::State Player::state() const
	{
		return state_;
	}

	bool Player::connected() const
	{
		return state_ >= Connected;
	}

	uint8_t Player::id() const
	{
		return id_;
	}

	ENetPeer *Player::peer() const
	{
		return peer_;
	}

	const char *Player::name() const
	{
		return name_;
	}
}
