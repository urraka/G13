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

				if (inputs_.size() == 1)
				{
					cmp::SoldierInput input;
					input.unserialize(inputs_[0]);
					soldier_.update(dt, &input);

					inputs_.clear();
				}
			}
			break;

			case Remote:
			{
				/*const int size = int(sizeof stateBuffer_ / sizeof stateBuffer_[0]);

				// TODO: interpolate
				// uint32_t drawTick = tick - size / 2;

				for (int i = size - 1; i >= 0; i--)
				{
					int idx = (stateBase_ + i) % size;

					if (stateBuffer_[idx].received)
					{
						soldier_.graphics.update(dt, stateBuffer_[idx].state);
						break;
					}
				}*/

				if (stateBuffer_[0].received)
					soldier_.graphics.update(dt, stateBuffer_[0].state);
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
					for (size_t i = 0; i < inputs_.size(); i++)
					{
						cmp::SoldierInput input;
						input.unserialize(inputs_[i]);
						soldier_.update(dt, &input);
					}

					inputs_.clear();
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
		lastInputTick_ = tick - 1;
		joinTick_ = tick;
		soldier_.reset(position);
		soldier_.map(map->collisionMap());
	}

	void Player::onSoldierState(uint32_t tick, const cmp::SoldierState &soldierState)
	{
		if (tick < stateTick_)
			return;

		stateBuffer_[0].state = soldierState;
		stateBuffer_[0].received = true;

		/*const int size = int(sizeof stateBuffer_ / sizeof stateBuffer_[0]);

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
		stateBuffer_[index].state = soldierState;*/
	}

	void Player::onInput(uint32_t tick, const cmp::SoldierInput &input)
	{
		if (tick > lastInputTick_)
		{
			inputs_.push_back(input.serialize());
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

	ent::Soldier *Player::soldier()
	{
		return &soldier_;
	}
}
