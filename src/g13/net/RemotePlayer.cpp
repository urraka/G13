#include "RemotePlayer.h"
#include <g13/coll/World.h>
#include <math/wrap_angle.h>
#include <assert.h>

namespace g13 {
namespace net {

RemotePlayer::RemotePlayer()
{
	bulletBuffer[0].reserve(10);
	bulletBuffer[1].reserve(10);
}

void RemotePlayer::initialize()
{
	Player::initialize();

	connectTick = -1;
	spawnTick   = -1;

	stateBuffer.clear();
	bulletBuffer[0].clear();
	bulletBuffer[1].clear();
}

void RemotePlayer::update(Time dt, int clientTick, coll::World &world)
{
	assert(state == Playing);

	cmp::SoldierState soldierState;

	soldier.entity.previous = soldier.entity.current;

	int desiredTick = std::max(clientTick, spawnTick);
	int renderedTick = desiredTick;

	int N = stateBuffer.size();
	int a = N - 1;
	int b = 0;

	while (a >= 0 && stateBuffer[a].tick > desiredTick) a--;
	while (b <  N && stateBuffer[b].tick < desiredTick) b++;

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
		const cmp::SoldierState *sa = &stateBuffer[a].data;
		const cmp::SoldierState *sb = &stateBuffer[b].data;

		soldierState = *sb;

		float span = (float)(stateBuffer[b].tick - stateBuffer[a].tick);
		fixed percent = fixed(float(desiredTick - stateBuffer[a].tick) / span);

		if (percent > 1)
		{
			renderedTick = stateBuffer[b].tick;

			fixvec2 dest = sb->position;
			fixvec2 diff = dest - soldier.physics.position;
			fixed length = fpm::length(diff);

			if (length > 1)
				soldierState.position = soldier.physics.position + diff * fixed(0.4);
		}
		else
		{
			renderedTick = glm::mix(stateBuffer[a].tick, stateBuffer[b].tick, fpm::to_float(percent));

			soldierState.position = sa->position + (sb->position - sa->position) * percent;

			if (sa->rightwards != sb->rightwards)
			{
				float a = M_PI * sa->angle / UINT16_MAX - M_PI / 2;
				float b = M_PI * sb->angle / UINT16_MAX - M_PI / 2;

				if (!sa->rightwards) a = -a + M_PI;
				if (!sb->rightwards) b = -b + M_PI;

				float angle = math::mix_angle(a, b, fpm::to_float(percent));
				angle = math::wrap_angle(angle, -M_PI / 2);

				soldierState.rightwards = (angle <= M_PI / 2);

				if (!soldierState.rightwards)
					angle = -angle + M_PI;

				soldierState.angle = (uint16_t)(UINT16_MAX * (angle + M_PI / 2) / M_PI);
			}
			else
			{
				soldierState.angle = glm::mix(sa->angle, sb->angle, fpm::to_float(percent));
			}
		}
	}
	else
	{
		renderedTick = stateBuffer[a].tick;
		soldierState = stateBuffer[a].data;
	}

	soldier.physics.position = soldierState.position;
	soldier.graphics.update(dt, soldierState);

	const fixrect &bboxNormal = soldier.physics.bboxNormal;
	const fixrect &bboxDucked = soldier.physics.bboxDucked;
	const fixrect &bbox = soldierState.duck ? bboxDucked : bboxNormal;

	soldier.entity.current = bbox + soldierState.position;

	world.add(&soldier.entity);

	fireBullets(renderedTick);
}

void RemotePlayer::onBullet(int tick, const cmp::BulletParams &params)
{
	bulletBuffer[0].push_back(Ticked<cmp::BulletParams>(tick, params));
}

void RemotePlayer::fireBullets(int tick)
{
	bulletBuffer[1].clear();
	bulletBuffer[1].reserve(bulletBuffer[0].size());

	for (size_t i = 0; i < bulletBuffer[0].size(); i++)
	{
		if (bulletBuffer[0][i].tick <= tick)
			soldier.createBulletCallback.fire(&bulletBuffer[0][i].data);
		else
			bulletBuffer[1].push_back(bulletBuffer[0][i]);
	}

	bulletBuffer[0].swap(bulletBuffer[1]);
}

}} // g13::net
