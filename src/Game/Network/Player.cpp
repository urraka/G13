#include "Player.h"
#include "../Map.h"

#include <hlp/assign.h>

namespace net
{
	Player::Player()
		:	mode_(Server),
			// stateBase_(0),
			// stateTick_(0),
			// stateTickLast_(0),
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
				uint32_t desiredTick = std::max(tick - stateBuffer_.capacity() / 2, joinTick_);

				int N = stateBuffer_.size();
				int a = N - 1;
				int b = 0;

				while (a >= 0 && stateBuffer_[a].tick > desiredTick) a--;
				while (b <  N && stateBuffer_[b].tick < desiredTick) b++;

				if (a == -1) a = 0;
				if (b ==  N) b = N - 1;

				if (a != b)
				{
					cmp::SoldierState sa = stateBuffer_[a].state;
					cmp::SoldierState sb = stateBuffer_[b].state;

					float span = (float)(stateBuffer_[b].tick - stateBuffer_[a].tick);
					float percent = (desiredTick - stateBuffer_[a].tick) / span;

					sa.position = sa.position + (sb.position - sa.position) * fixed(percent);

					soldier_.graphics.update(dt, sa);
				}
				else
				{
					soldier_.graphics.update(dt, stateBuffer_[a].state);
				}

				// first real interpolation test (broken)
				/*const int size = int(sizeof stateBuffer_ / sizeof stateBuffer_[0]);

				uint32_t desiredTick = std::max(tick - size / 2, joinTick_);
				int offset = std::min(size - 1, std::max(0, int(desiredTick - stateTick_)));

				int a = offset;
				int b = offset;

				while (a >= 0   && !stateBuffer_[(stateBase_ + a) % size].received) a--;
				while (b < size && !stateBuffer_[(stateBase_ + b) % size].received) b++;

				if (a == -1 || b == size)
				{
					soldier_.graphics.update(dt, soldier_.state());
					return;
				}

				if (a == -1)   a = b;
				if (b == size) b = a;

				if (b != size)
				{
					if (a != b)
					{
						assert(a < offset && offset < b);

						cmp::SoldierState sa = stateBuffer_[(stateBase_ + a) % size].state;
						cmp::SoldierState sb = stateBuffer_[(stateBase_ + b) % size].state;

						fixed percent = fixed(offset - a) / fixed(b - a);
						sa.position = sa.position + (sb.position - sa.position) * percent;

						// much less of a lame test:
						// const vec2 &current = soldier_.graphics.position.current;
						// vec2 diff = math::from_fixed(sa.position) - current;
						// float distance = glm::length(diff);

						// if (distance > 0.1f)
						// {
						// 	vec2 pos = current + diff * 0.1f;
						// 	sa.position.x = fixed(pos.x);
						// 	sa.position.y = fixed(pos.y);
						// }

						soldier_.graphics.update(dt, sa);
					}
					else
					{
						soldier_.graphics.update(dt, stateBuffer_[(stateBase_ + a) % size].state);
					}
				}*/

				// less of a lame test:
				// for (int i = size - 1; i >= 0; i--)
				// {
				// 	int idx = (stateBase_ + i) % size;

				// 	if (stateBuffer_[idx].received)
				// 	{
				// 		soldier_.graphics.update(dt, stateBuffer_[idx].state);
				// 		break;
				// 	}
				// }

				// lame test:
				// if (stateBuffer_[0].received)
				// 	soldier_.graphics.update(dt, stateBuffer_[0].state);
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

		// stateBase_ = 0;
		// stateTick_ = 0;
		// stateTickLast_ = 0;
	}

	void Player::onJoin(uint32_t tick, const Map *map, const fixvec2 &position)
	{
		state_ = Playing;
		lastInputTick_ = tick - 1;
		joinTick_ = tick;
		soldier_.reset(position);
		soldier_.map(map->collisionMap());

		stateBuffer_.clear();
		stateBuffer_.push(SoldierState(tick, soldier_.state()));

		// onSoldierState(tick, soldier_.state());
	}

	void Player::onSoldierState(uint32_t tick, const cmp::SoldierState &soldierState)
	{
		if (stateBuffer_.size() == 0 || tick > stateBuffer_[stateBuffer_.size() - 1].tick)
			stateBuffer_.push(SoldierState(tick, soldierState));

		// if (tick <= stateTickLast_)
		// 	return;

		// lame test:
		// stateBuffer_[0].state = soldierState;
		// stateBuffer_[0].received = true;

		// const int size = int(sizeof stateBuffer_ / sizeof stateBuffer_[0]);

		// int offset = tick - stateTick_;

		// if (offset >= size)
		// {
		// 	for (int i = 0; i < offset - size && i < size; i++)
		// 		stateBuffer_[(stateBase_ + i) % size].received = false;

		// 	stateBase_ = (stateBase_ + offset - size + 1) % size;
		// 	stateTick_ = tick - size + 1;
		// }

		// int index = (stateBase_ + offset) % size;
		// stateBuffer_[index].received = true;
		// stateBuffer_[index].state = soldierState;

		// stateTickLast_ = tick;
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
