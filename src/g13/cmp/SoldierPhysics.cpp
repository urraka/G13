#include "SoldierPhysics.h"
#include "SoldierInput.h"

#include <g13/coll/World.h>
#include <g13/coll/Result.h>
#include <assert.h>

namespace g13 {
namespace cmp {

SoldierPhysics::SoldierPhysics()
	:	bboxNormal(-17, -66, 17, 0),
		bboxDucked(-17, -50, 17, 0),
		ducked_(false),
		segment_(0)
{
}

void SoldierPhysics::update(Time dt, const coll::World &world, const SoldierInput &input)
{
	fixed dts = fixed((int)dt / 1000) / fixed(1000);
	const fixed kGravity = world.gravity();
	const fixed kJumpVel = fixed(-700);
	const fixed kWalkVel = fixed(500);
	// const fixed kDuckVel = fixed(150);
	const fixed kRunVel  = fixed(800);

	if (ducked_ && !input.duck)
	{
		fixvec2 offset = fixvec2(0, bboxDucked.height() - bboxNormal.height());
		ducked_ = (world.collision(position, position + offset, bboxDucked).segment != 0);
	}
	else
	{
		ducked_ = input.duck;
	}

	const fixrect &bbox = bounds();

	if (input.jump && floor())
	{
		const fixvec2 offset = fixvec2(0, -1);
		coll::Result collision = world.collision(position, position + offset, bbox);

		if (!collision.segment || fpm::fabs(fpm::slope(collision.segment->line)) > 1)
		{
			velocity.y = kJumpVel;
			segment_ = 0;

			if (input.right || input.left)
			{
				const fixed direction = (input.left ? -1 : 1);

				if (walkvel == 0 || (direction == fpm::sign(walkvel) && fpm::fabs(walkvel) < kWalkVel))
					walkvel = kWalkVel * direction;
			}
		}
	}

	if (input.right || input.left)
	{
		const fixed limit = floor() ? (input.run ? kRunVel : kWalkVel) : kWalkVel;
		const fixed direction = (input.left ? -1 : 1);

		bool dirchange = fpm::sign(walkvel) != direction;

		if (fpm::fabs(walkvel) < limit || dirchange)
		{
			const fixed acc = floor() ? (dirchange ? 2200 : 1800) : 800;
			walkvel += acc * direction * dts;
			walkvel = fpm::max(-limit, fpm::min(limit, walkvel));
		}
		else if (floor())
		{
			fixed wv = walkvel - fixed(800) * fpm::sign(walkvel) * dts;

			if (fpm::sign(wv) != fpm::sign(walkvel) || fpm::fabs(wv) < limit)
				wv = limit * direction;

			walkvel = wv;
		}
	}
	else if (walkvel != 0)
	{
		fixed wv = walkvel - fixed(floor() ? 2200 : 800) * fpm::sign(walkvel) * dts;

		if (fpm::sign(wv) != fpm::sign(walkvel))
			wv = 0;

		walkvel = wv;
	}

	acceleration.x = 0;
	acceleration.y = kGravity;

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

		const coll::Segment *prevSegment = segment_;
		coll::Hull prevHull = hull_;

		bool wasInFloor = segment_ && segment_->floor;

		if (segment_ != 0 && segment_->floor)
		{
			// adjust velocity to floor direction

			delta.y = 0;
			velocity.y = 0;

			if (delta.x != 0)
			{
				direction = segment_->line.p2 - segment_->line.p1;
				const fixvec2 *dest = &segment_->line.p2;

				if (fpm::sign(direction.x) != fpm::sign(delta.x))
				{
					direction = -direction;
					dest = &segment_->line.p1;
				}

				fixed length = fpm::fabs(delta.x);
				delta = fpm::normalize(direction) * length;

				if (fpm::sign(dest->x - position.x) != fpm::sign(dest->x - (position.x + delta.x)))
				{
					delta.y = (dest->x - position.x) * delta.y / delta.x;
					delta.x = dest->x - position.x;

					if (false && input.run && !ducked_)
					{
						segment_ = 0;
					}
					else
					{
						const coll::Segment *nextSegment;

						if (dest == &segment_->line.p1)
							nextSegment = segment_->prev;
						else
							nextSegment = segment_->next;

						if (nextSegment != 0 && !nextSegment->floor)
							nextSegment = 0;

						if (nextSegment != 0 && !hull_.owns(nextSegment))
							hull_ = coll::Hull(*nextSegment, bbox);

						segment_ = nextSegment;
					}

					nextDelta.x = fpm::max(0, length - fpm::length(delta)) * fpm::sign(delta.x);
				}
			}
		}
		else if (segment_ != 0)
		{
			// wall/roof collision

			fixvec2 normal = fpm::normal(segment_->line);

			if (fpm::dot(velocity, normal) < 0)
			{
				direction = fpm::normalize(segment_->line.p2 - segment_->line.p1);
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
					if (segment_ != prevSegment)
					{
						segment_ = prevSegment;
						hull_ = prevHull;
					}
				}
				else
				{
					if (lastCollision == 0 || *lastCollision != collision.position)
					{
						dts = dts * (fixed::one - collision.percent);

						if (direction == fixvec2(0, 0))
							direction = normalize(delta);
						else if (fpm::sign(direction) != fpm::sign(delta))
							direction = -direction;

						fixed length = fpm::length(delta);
						length = fpm::max(0, (length - length * collision.percent));

						nextDelta = direction * length;
					}

					hull_ = collision.hull;
					segment_ = &hull_.segments[collision.index];

					// sometimes we hit a floor line but position is out of its bounds, derp
					// the next if hacks together a fix for that, hopefully without side effects

					if (segment_->floor)
					{
						const fixvec2 *a = &segment_->line.p1;
						const fixvec2 *b = &segment_->line.p2;

						if (a->x > b->x)
							std::swap(a, b);

						const coll::Segment *nextSegment = 0;

						if (position.x < a->x)
						{
							if (a == &segment_->line.p1)
								nextSegment = segment_->prev;
							else
								nextSegment = segment_->next;
						}
						else if (position.x > b->x)
						{
							if (b == &segment_->line.p1)
								nextSegment = segment_->prev;
							else
								nextSegment = segment_->next;
						}

						if (nextSegment != 0)
						{
							if (!nextSegment->floor)
							{
								nextSegment = 0;
							}
							else
							{
								if (!hull_.owns(nextSegment))
									hull_ = coll::Hull(*nextSegment, bbox);

								segment_ = nextSegment;
							}
						}
					}
				}
			}
			else if (segment_ != 0 && !segment_->floor)
			{
				segment_ = 0;
			}

			lastCollisionValue = collision.position;
			lastCollision = &lastCollisionValue;
		}
	}
}

void SoldierPhysics::reset(fixvec2 pos)
{
	position = pos;
	velocity.x = 0;
	velocity.y = 0;
	acceleration.x = 0;
	acceleration.y = 0;
	walkvel = 0;

	ducked_ = false;
	segment_ = 0;
}

bool SoldierPhysics::ducking() const
{
	return ducked_;
}

bool SoldierPhysics::floor() const
{
	return segment_ != 0 && segment_->floor;
}

}} // g13::cmp
