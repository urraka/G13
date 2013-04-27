#include "../Game.h"
#include "SoldierInput.h"
#include "SoldierPhysics.h"

#include <assert.h>

SoldierPhysics::SoldierPhysics() : input(0), map(0), ducking_(false), currentNode_(0) {}

void SoldierPhysics::update(Time dt)
{
	assert(map != 0);
	assert(input != 0);

	const fixed dts = fixed((int)dt / 1000) / fixed(1000);
	const fixed kGravity = fixed(1470); // 9.8 * 150
	const fixed kJumpVel = fixed(-550);
	const fixed kWalkVel = fixed(250);
	const fixed kDuckVel = fixed(150);
	const fixed kRunVel  = fixed(300);

	acceleration.x = fixed::zero;
	acceleration.y = kGravity;
	velocity.x = input->right ? kWalkVel : input->left ? -kWalkVel : fixed::zero;
	velocity.y += acceleration.y * dts;

	if (input->jump && currentNode_ != 0 && currentNode_->floor)
	{
		velocity.y = kJumpVel;
		currentNode_ = 0;
	}

	fixvec2 nextDelta = velocity * dts;

	while (nextDelta.x != fixed::zero || nextDelta.y != fixed::zero)
	{
		fixvec2 delta = nextDelta;
		fixvec2 direction;

		nextDelta.x = fixed::zero;
		nextDelta.y = fixed::zero;

		if (currentNode_ != 0 && currentNode_->floor)
		{
			// adjust velocity to floor direction

			delta.y = fixed::zero;
			velocity.y = fixed::zero;

			if (delta.x != fixed::zero)
			{
				direction = currentNode_->line.p2 - currentNode_->line.p1;
				const fixvec2 *dest = &currentNode_->line.p2;

				if (fpm::sign(direction.x) != fpm::sign(delta.x))
				{
					direction = -direction;
					dest = &currentNode_->line.p1;
				}

				fixed length = fpm::fabs(delta.x);
				delta = fpm::normalize(direction) * length;

				if (fpm::sign(dest->x - position.x) != fpm::sign(dest->x - (position.x + delta.x)))
				{
					delta.y = (dest->x - position.x) * delta.y / delta.x;
					delta.x = dest->x - position.x;

					const Collision::Node *nextNode = (dest == &currentNode_->line.p1 ? currentNode_->prev : currentNode_->next);

					if (nextNode != 0 && !nextNode->floor)
						nextNode = 0;

					if (nextNode != 0 && (nextNode < &currentHull_.nodes[0] || nextNode > &currentHull_.nodes[2]))
						currentHull_ = Collision::createHull(nextNode, bbox);

					currentNode_ = nextNode;
					nextDelta.x = fpm::max(fixed::zero, length - fpm::length(delta)) * fpm::sign(delta.x);
				}
			}
		}
		else if (currentNode_ != 0)
		{
			// wall/roof collision

			fixvec2 normal = fpm::normal(currentNode_->line);

			if (fpm::dot(delta, normal) < fixed::zero)
			{
				direction = fpm::normalize(currentNode_->line.p2 - currentNode_->line.p1);
				fixed length = fpm::dot(direction, delta);
				fixvec2 vel = direction * length;

				if (fpm::fabs(vel.y) > fpm::fabs(delta.y))
					vel *= fpm::fabs(delta.y / vel.y);

				if (fpm::dot(normal, fixvec2(fixed::zero, -fixed::one)) <= fixed::zero)
					velocity.y = vel.y / dts;

				delta = vel;
			}
		}

		if (delta.x != fixed::zero || delta.y != fixed::zero)
		{
			Collision::Result collision = Collision::resolve(*map, position, position + delta, bbox);
			position = collision.position;

			if (collision.node)
			{
				if (currentNode_ != 0 && currentNode_->floor && !collision.hull.nodes[collision.iHullNode].floor)
				{
					velocity.x = fixed::zero;
					nextDelta.x = fixed::zero;
				}
				else
				{
					// TODO: maybe even better would be to check against last collision.position, if "equal" abort
					if (currentNode_ == 0)
					{
						if (direction == fixvec2(fixed::zero, fixed::zero))
							direction = normalize(delta);

						fixed length = fpm::length(delta);
						nextDelta = direction * fpm::max(fixed::zero, (length - length * collision.percent));
					}

					currentHull_ = collision.hull;
					currentNode_ = &currentHull_.nodes[collision.iHullNode];
				}
			}
			else if (currentNode_ != 0 && !currentNode_->floor)
			{
				currentNode_ = 0;
			}
		}
	}
}

void SoldierPhysics::teleport(fixvec2 pos)
{
	position = pos;
	velocity.x = fixed::zero;
	velocity.y = fixed::zero;
	acceleration.x = fixed::zero;
	acceleration.y = fixed::zero;

	ducking_ = false;
	currentNode_ = 0;
}
