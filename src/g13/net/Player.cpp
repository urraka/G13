#include "Player.h"

#include <g13/Map.h>
#include <g13/coll/collision.h>
#include <g13/callback.h>

#include <hlp/assign.h>
#include <math/mix_angle.h>
#include <math/wrap_angle.h>

namespace g13 {
namespace net {

Player::Player()
	:	peer_(0)
{
	hlp::assign(name_, "");
}

void Player::initialize()
{
	onDisconnect(0);
}

void Player::updateLocal(Time dt)
{
	// TODO: add input to a buffer to correct position with a replay

	if (inputs_.size() == 1)
	{
		soldier_.collisionEntity.previous = soldier_.physics.bounds() + soldier_.physics.position;

		soldier_.update(dt, &inputs_[0].data);
		inputs_.clear();

		soldier_.collisionEntity.current = soldier_.physics.bounds() + soldier_.physics.position;
	}
}

void Player::updateRemote(Time dt, int tick)
{
	cmp::SoldierState state;

	soldier_.collisionEntity.previous = soldier_.collisionEntity.current;

	int desiredTick = std::max(tick, joinTick_);
	int renderedTick = desiredTick;

	int N = stateBuffer_.size();
	int a = N - 1;
	int b = 0;

	while (a >= 0 && stateBuffer_[a].tick > desiredTick) a--;
	while (b <  N && stateBuffer_[b].tick < desiredTick) b++;

	#ifdef DEBUG
		if (!dbg->interpolation)
			a = b = stateBuffer_.size() - 1;
	#endif

	if (a == -1) a = 0;

	if (b == N)
	{
		b = N - 1;

		if (a > 0 && b > 0)
			a = b - 1;

		// TODO: limit desired tick value?
	}

	if (a < b)
	{
		// interpolate/extrapolate

		const cmp::SoldierState *sa = &stateBuffer_[a].data;
		const cmp::SoldierState *sb = &stateBuffer_[b].data;

		state = *sb;

		float span = (float)(stateBuffer_[b].tick - stateBuffer_[a].tick);
		fixed percent = fixed(float(desiredTick - stateBuffer_[a].tick) / span);

		if (percent > 1)
		{
			renderedTick = stateBuffer_[b].tick;

			fixvec2 dest = sb->position;

			#ifdef DEBUG
				if (dbg->extrapolation)
				{
					dest = sa->position + (sb->position - sa->position) * percent;

					const fixrect *bboxNormal = &soldier_.physics.bboxNormal;
					const fixrect *bboxDucked = &soldier_.physics.bboxDucked;
					const fixrect *bbox = state.duck ? bboxDucked : bboxNormal;

					coll::Result result = soldier_.physics.world->collision(state.position, dest, *bbox);

					if (result.segment != 0)
						dest = result.position;
				}
			#endif

			fixvec2 pos = to_fixed(soldier_.graphics.position.current);
			fixvec2 diff = dest - pos;
			fixed length = fpm::length(diff);

			if (length > 1)
				state.position = pos + diff * fixed(0.4);
		}
		else
		{
			renderedTick = glm::mix(stateBuffer_[a].tick, stateBuffer_[b].tick, percent.to_float());

			state.position = sa->position + (sb->position - sa->position) * percent;

			if (sa->rightwards != sb->rightwards)
			{
				float a = M_PI * sa->angle / UINT16_MAX - M_PI / 2;
				float b = M_PI * sb->angle / UINT16_MAX - M_PI / 2;

				if (!sa->rightwards) a = -a + M_PI;
				if (!sb->rightwards) b = -b + M_PI;

				float angle = math::mix_angle(a, b, percent.to_float());
				angle = math::wrap_angle(angle, -M_PI / 2);

				state.rightwards = (angle <= M_PI / 2);

				if (!state.rightwards)
					angle = -angle + M_PI;

				state.angle = (uint16_t)(UINT16_MAX * (angle + M_PI / 2) / M_PI);
			}
			else
			{
				state.angle = glm::mix(sa->angle, sb->angle, percent.to_float());
			}
		}
	}
	else
	{
		renderedTick = stateBuffer_[a].tick;
		state = stateBuffer_[a].data;
	}


	std::deque<BulletParams>::iterator it = bullets_.begin();

	while (it != bullets_.end() && it->tick <= renderedTick)
	{
		soldier_.createBulletCallback->fire(&(it->data));

		bullets_.pop_front();
		it = bullets_.begin();
	}

	// update collision entity
	{
		const fixrect &bboxNormal = soldier_.physics.bboxNormal;
		const fixrect &bboxDucked = soldier_.physics.bboxDucked;
		const fixrect &bbox = state.duck ? bboxDucked : bboxNormal;

		soldier_.collisionEntity.current = bbox + to_fixed(soldier_.graphics.position.current);
	}

	soldier_.graphics.update(dt, state);
}

void Player::updateServer(Time dt, int tick)
{
	if (state_ == Connecting)
	{
		connectTimeout_ += dt;

		if (connectTimeout_ >= sys::time<sys::Seconds>(5))
		{
			enet_peer_reset(peer_);
			onDisconnect(disconnectTick_);
		}
	}
	else
	{
		soldier_.collisionEntity.previous = soldier_.physics.bounds() + soldier_.physics.position;

		for (size_t i = 0; i < inputs_.size() && inputs_[i].tick <= tick; i++, tick_++)
			soldier_.update(dt, &inputs_[i].data);

		soldier_.collisionEntity.current = soldier_.physics.bounds() + soldier_.physics.position;

		inputs_.clear();
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

void Player::onDisconnect(int tick)
{
	state_ = Disconnected;
	peer_ = 0;
	disconnectTick_ = tick;

	// let go all the bullets

	std::deque<BulletParams>::iterator it = bullets_.begin();

	while (it != bullets_.end())
	{
		soldier_.createBulletCallback->fire(&(it->data));

		bullets_.pop_front();
		it = bullets_.begin();
	}
}

void Player::onJoin(int tick, const Map *map, const fixvec2 &position)
{
	state_ = Playing;
	lastInputTick_ = tick - 1;
	tick_ = joinTick_ = tick;
	soldier_.reset(position);
	soldier_.world(map->world());

	inputs_.clear();
	stateBuffer_.clear();
}

void Player::onSoldierState(int tick, const cmp::SoldierState &soldierState)
{
	if (stateBuffer_.size() == 0 || tick > stateBuffer_[stateBuffer_.size() - 1].tick)
	{
		stateBuffer_.push(SoldierState(tick, soldierState));

		if (stateBuffer_.size() == 1)
		{
			coll::Entity &entity = soldier_.collisionEntity;
			const cmp::SoldierPhysics *physics = &soldier_.physics;

			const fixrect &bbox = soldierState.duck ? physics->bboxDucked : physics->bboxNormal;

			entity.current = entity.previous = bbox + soldierState.position;
		}
	}
}

void Player::onInput(int tick, const cmp::SoldierInput &input)
{
	if (tick > lastInputTick_)
	{
		inputs_.push_back(SoldierInput(tick, input));
		lastInputTick_ = tick;
	}
}

void Player::onBulletCreated(int tick, const cmp::BulletParams &prams)
{
	cmp::BulletParams params = prams;

	if (tick <= disconnectTick_)
		soldier_.createBulletCallback->fire(&params); // playerid shouldn't matter on client
	else
		bullets_.push_back(BulletParams(tick, params));
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

int Player::tick() const
{
	return tick_;
}

}} // g13::net
