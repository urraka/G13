#include "SoldierPhysics.h"
#include "SoldierInput.h"

#include <g13/vars.h>
#include <g13/coll/World.h>
#include <g13/coll/Result.h>
#include <assert.h>

namespace g13 {
namespace cmp {

SoldierPhysics::SoldierPhysics()
	:	bboxNormal(-17, -66, 17, 0),
		bboxDucked(-17, -50, 17, 0)
{
	reset(fixvec2(0));
}

void SoldierPhysics::update(Time dt, const coll::World &world, const SoldierInput &input)
{
	switch (mode)
	{
		case Normal: updateNormal(dt, world, input); break;
		case Rope: updateRope(dt, world, input); break;
	}
}

void SoldierPhysics::hook(const fixvec2 &hookPosition)
{
	mode = Rope;
	ropeHook = hookPosition;
	// position += bounds().center() - bboxDucked.center();
	ducked = true;
	segment = 0;
}

void SoldierPhysics::unhook()
{
	mode = Normal;
	walkvel = velocity.x;
	segment = 0;
}

void SoldierPhysics::updateNormal(Time dt, const coll::World &world, const SoldierInput &input)
{
	fixed dts = to_fixed(dt);

	if (ducked && !input.duck)
	{
		fixvec2 offset = fixvec2(0, bboxDucked.height() - bboxNormal.height());
		ducked = (world.collision(position, position + offset, bboxDucked).segment != 0);
	}
	else
	{
		ducked = input.duck;
	}

	const fixrect &bbox = bounds();

	if (input.jump && floor())
	{
		const fixvec2 offset = fixvec2(0, -1);
		coll::Result collision = world.collision(position, position + offset, bbox);

		if (!collision.segment || fpm::fabs(fpm::slope(collision.segment->line)) > 1)
		{
			velocity.y = vars::JumpVel;
			segment = 0;

			if (input.right || input.left)
			{
				const fixed direction = (input.left ? -1 : 1);

				if (walkvel == 0 || (direction == fpm::sign(walkvel) && fpm::fabs(walkvel) < vars::WalkVel))
					walkvel = vars::WalkVel * direction;
			}
		}
	}

	if (input.right || input.left)
	{
		const fixed limit = floor() ? (input.run ? vars::RunVel : vars::WalkVel) : vars::AirMoveVel;
		const fixed direction = (input.left ? -1 : 1);

		bool dirchange = fpm::sign(walkvel) != direction;

		if (fpm::fabs(walkvel) < limit || dirchange)
		{
			const fixed acc = floor() ? (dirchange ? vars::BreakAcc : vars::MoveAcc) : vars::AirMoveAcc;

			walkvel += acc * direction * dts;
			walkvel = fpm::max(-limit, fpm::min(limit, walkvel));
		}
		else if (floor())
		{
			fixed wv = walkvel - fixed(vars::LimitAcc) * fpm::sign(walkvel) * dts;

			if (fpm::sign(wv) != fpm::sign(walkvel) || fpm::fabs(wv) < limit)
				wv = limit * direction;

			walkvel = wv;
		}
	}
	else if (walkvel != 0)
	{
		const fixed &acc = floor() ? vars::BreakAcc : vars::AirBreakAcc;

		fixed wv = walkvel - acc * fpm::sign(walkvel) * dts;

		if (fpm::sign(wv) != fpm::sign(walkvel))
			wv = 0;

		walkvel = wv;
	}

	acceleration.x = 0;
	acceleration.y = vars::Gravity;

	velocity.x = walkvel;
	velocity.y += acceleration.y * dts;

	fixvec2 nextDelta = velocity * dts;
	fixvec2 lastCollisionValue;
	fixvec2 *lastCollision = 0;

	while (nextDelta != fixvec2(0, 0))
	{
		fixvec2 delta = nextDelta;
		fixvec2 direction;

		nextDelta = fixvec2(0, 0);

		const coll::Segment *prevSegment = segment;
		coll::Hull prevHull = hull;

		bool wasInFloor = false;

		if (floor())
		{
			wasInFloor = true;

			// adjust velocity to floor direction

			delta.y = 0;
			velocity.y = 0;

			if (delta.x != 0)
			{
				direction = segment->line.p2 - segment->line.p1;
				const fixvec2 *dest = &segment->line.p2;

				if (fpm::sign(direction.x) != fpm::sign(delta.x))
				{
					direction = -direction;
					dest = &segment->line.p1;
				}

				const fixed length = fpm::fabs(delta.x);
				const fixvec2 udir = fpm::normalize(direction);

				delta = udir * length;

				if (fpm::sign(dest->x - position.x) != fpm::sign(dest->x - (position.x + delta.x)))
				{
					delta.y = (dest->x - position.x) * delta.y / delta.x;
					delta.x = dest->x - position.x;

					const coll::Segment *nextSegment;

					if (dest == &segment->line.p1)
						nextSegment = segment->prev;
					else
						nextSegment = segment->next;

					if (nextSegment != 0 && !nextSegment->floor)
						nextSegment = 0;

					if (nextSegment != 0 && !hull.owns(nextSegment))
					{
						hull = coll::Hull(*nextSegment, bbox);
						nextSegment = &hull.segments[0];
					}

					segment = nextSegment;

					if (segment != 0)
					{
						nextDelta.x = fpm::max(0, length - fpm::length(delta)) * fpm::sign(delta.x);
					}
					else
					{
						velocity = udir * fpm::fabs(walkvel);
						delta = udir * length;
						walkvel = velocity.x;
					}
				}
			}
		}
		else if (segment != 0)
		{
			// wall/roof collision

			fixvec2 normal = fpm::normal(segment->line);

			if (fpm::dot(velocity, normal) < 0)
			{
				direction = fpm::normalize(segment->line.p2 - segment->line.p1);
				fixed length = fpm::dot(direction, velocity);
				fixvec2 vel = direction * length;

				if (fpm::fabs(vel.y) > fpm::fabs(velocity.y))
					vel *= fpm::fabs(velocity.y / vel.y);

				if (fpm::dot(normal, fixvec2(0, -1)) <= 0)
					velocity = vel;

				delta = vel * dts;

				walkvel = vel.x;
			}
		}

		if (delta != fixvec2(0, 0))
		{
			coll::Result collision = world.collision(position, position + delta, bbox);
			position = collision.position;

			if (collision.segment)
			{
				if (wasInFloor && !collision.hull.segments[collision.index].floor)
				{
					walkvel = 0;
					velocity.x = 0;
					nextDelta.x = 0;

					// revert segment in case we switched to the next one but collided
					// with a wall before actually reaching it
					if (segment != prevSegment)
					{
						segment = prevSegment;
						hull = prevHull;
					}
				}
				else
				{
					if (lastCollision == 0 || *lastCollision != collision.position)
					{
						dts = dts * (fpm::One - collision.percent);

						if (direction == fixvec2(0, 0))
							direction = normalize(delta);
						else if (fpm::sign(direction) != fpm::sign(delta))
							direction = -direction;

						fixed length = fpm::length(delta);
						length = fpm::max(0, (length - length * collision.percent));

						nextDelta = direction * length;
					}

					hull = collision.hull;
					segment = &hull.segments[collision.index];

					// Sometimes we hit a floor line but position is out of its bounds...
					// The next 'if' hacks together a fix for that, hopefully without side effects.
					// I don't remember what this means but i have the feeling that it's a precision
					// issue. We hit a floor and when resolving collision the position goes out of
					// the floor bounds by a very small amount, which fucks up the code that makes
					// you walk along the floor.

					if (segment->floor)
					{
						const fixvec2 *a = &segment->line.p1;
						const fixvec2 *b = &segment->line.p2;

						if (a->x > b->x)
							std::swap(a, b);

						const coll::Segment *nextSegment = 0;

						if (position.x < a->x)
						{
							if (a == &segment->line.p1)
								nextSegment = segment->prev;
							else
								nextSegment = segment->next;
						}
						else if (position.x > b->x)
						{
							if (b == &segment->line.p1)
								nextSegment = segment->prev;
							else
								nextSegment = segment->next;
						}

						if (nextSegment != 0)
						{
							if (!nextSegment->floor)
							{
								nextSegment = 0;
							}
							else
							{
								if (!hull.owns(nextSegment))
								{
									hull = coll::Hull(*nextSegment, bbox);
									nextSegment = &hull.segments[0];
								}

								segment = nextSegment;
							}
						}
					}
				}
			}
			else if (segment != 0 && !segment->floor)
			{
				segment = 0;
			}

			lastCollisionValue = collision.position;
			lastCollision = &lastCollisionValue;
		}
	}
}

void SoldierPhysics::updateRope(Time deltaTime, const coll::World &world, const SoldierInput &input)
{
	const fixed delta = to_fixed(deltaTime);

	fixed dt = delta;
	fixvec2 initialpos = position;
	fixvec2 prevpos = position;
	fixvec2 ropeacc = 15 * (ropeHook - position);

	acceleration = ropeacc + fixvec2(0, vars::Gravity);
	velocity += acceleration * dt;
	position += velocity * dt;

	fixed L = fpm::length(position - ropeHook);

	if (L > vars::MaxRopeLength)
		position = ropeHook + ((position - ropeHook) / L) * vars::MaxRopeLength;

	coll::Result collision = world.collision(prevpos, position, bounds());
	position = collision.position;

	int loopCount = 0;

	while (collision.segment != 0 && loopCount++ < 3)
	{
		segment = collision.segment;
		hull = collision.hull;

		dt *= (1 - collision.percent);

		// const fixline &line = collision.segment->line;
		const fixline &line = collision.hull.segments[collision.index].line;
		const fixvec2 normal = fpm::normal(line);
		const fixvec2 invvel = fpm::normalize(-velocity);

		// if (fpm::dot(normal, invvel) < 1) que no se trabe la cuerdita ejje
		if (fpm::dot(velocity, normal) < 0)
		{
			// const fixed scalar = fpm::lerp(fpm::fabs(velocity.x), fpm::fabs(velocity.y), fpm::Half);
			const fixed scalar = 1;

			fixvec2 direction = fpm::normalize(line.p2 - line.p1);
			fixed length = fpm::dot(direction, velocity / scalar);

			velocity = direction * length * scalar;

			prevpos = position;
			position += velocity * dt;

			collision = world.collision(prevpos, position, bounds());
			position = collision.position;
		}
		else
		{
			break;
		}
	}

	velocity = (position - initialpos) / delta;
}

void SoldierPhysics::reset(fixvec2 pos)
{
	mode = Normal;
	position = pos;
	velocity.x = 0;
	velocity.y = 0;
	acceleration.x = 0;
	acceleration.y = 0;
	walkvel = 0;
	ducked = false;
	segment = 0;
}

}} // g13::cmp
