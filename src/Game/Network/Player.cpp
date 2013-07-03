#include "Player.h"
#include "../Map.h"
#include "../Debugger.h"

#include <hlp/assign.h>

namespace net
{
	Player::Player()
		:	mode_(Server),
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
				const uint32_t ticksBehind = 4; //stateBuffer_.capacity() / 2;
				uint32_t desiredTick = std::max(tick - ticksBehind, joinTick_);

				int N = stateBuffer_.size();
				int a = N - 1;
				int b = 0;

				while (a >= 0 && stateBuffer_[a].tick > desiredTick) a--;
				while (b <  N && stateBuffer_[b].tick < desiredTick) b++;

				if (a == -1) a = 0;
				if (b ==  N) b = N - 1;

				DBG(
					if (!dbg->interpolation)
						a = b = stateBuffer_.size() - 1;
				);

				cmp::SoldierState state;

				if (a != b)
				{
					assert(a < b);

					const cmp::SoldierState *sa = &stateBuffer_[a].state;
					const cmp::SoldierState *sb = &stateBuffer_[b].state;

					float span = (float)(stateBuffer_[b].tick - stateBuffer_[a].tick);
					float percent = float(desiredTick - stateBuffer_[a].tick) / span;

					state = *sb;
					state.position = sa->position + (sb->position - sa->position) * fixed(percent);
				}
				else
				{
					state = stateBuffer_[a].state;
				}

				soldier_.graphics.update(dt, state);
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
						tick_++;
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
		tick_ = joinTick_ = tick;
		soldier_.reset(position);
		soldier_.map(map->collisionMap());

		stateBuffer_.clear();
		stateBuffer_.push(SoldierState(tick, soldier_.state()));
	}

	void Player::onSoldierState(uint32_t tick, const cmp::SoldierState &soldierState)
	{
		if (stateBuffer_.size() == 0 || tick > stateBuffer_[stateBuffer_.size() - 1].tick)
			stateBuffer_.push(SoldierState(tick, soldierState));
		else
			DBG(std::cout << "tick #" << tick << " skipped." << std::endl)
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

	uint32_t Player::tick() const
	{
		return tick_;
	}
}
