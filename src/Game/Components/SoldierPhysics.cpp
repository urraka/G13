#include "../Game.h"
#include "SoldierInput.h"
#include "SoldierPhysics.h"

#include <assert.h>

namespace cmp
{
	SoldierPhysics::SoldierPhysics()
		:	bboxNormal(-17, -66, 17, 0),
			bboxDucked(-17, -50, 17, 0),
			input(0),
			map(0),
			ducked_(false),
			currentNode_(0)
	{
	}

	void SoldierPhysics::update(Time dt)
	{
		assert(map != 0);
		assert(input != 0);

		fixed dts = fixed((int)dt / 1000) / fixed(1000);
		const fixed kGravity = fixed(1470); // 9.8 * 150
		const fixed kJumpVel = fixed(-550);
		const fixed kWalkVel = fixed(250);
		const fixed kDuckVel = fixed(150);
		const fixed kRunVel  = fixed(350);

		if (ducked_ && !input->duck)
		{
			fixvec2 offset = fixvec2(0, bboxDucked.height() - bboxNormal.height());
			ducked_ = Collision::resolve(map, position, position + offset, bboxDucked).node != 0;
		}
		else
		{
			ducked_ = input->duck;
		}

		fixrect bbox = ducked_ ? bboxDucked : bboxNormal;

		if (input->jump && floor())
		{
			const fixvec2 offset = fixvec2(0, -1);
			Collision::Result collision = Collision::resolve(map, position, position + offset, bbox);

			if (!collision.node || fpm::fabs(fpm::slope(collision.node->line)) > 1)
			{
				velocity.y = kJumpVel;
				currentNode_ = 0;
			}
		}

		acceleration.x = 0;
		acceleration.y = kGravity;

		velocity.x = 0;
		velocity.y += acceleration.y * dts;

		if (input->right || input->left)
		{
			fixed vel = (ducked_ && floor()) ? kDuckVel : input->run ? kRunVel : kWalkVel;
			velocity.x = input->left ? -vel : vel;
		}

		fixvec2 nextDelta = velocity * dts;
		fixvec2 lastCollisionValue, *lastCollision = 0;

		while (nextDelta != fixvec2(0, 0))
		{
			fixvec2 delta = nextDelta;
			fixvec2 direction;

			nextDelta = fixvec2(0, 0);

			const Collision::Node *prevNode = currentNode_;
			Collision::Hull prevHull = currentHull_;

			bool wasInFloor = currentNode_ && currentNode_->floor;

			if (currentNode_ != 0 && currentNode_->floor)
			{
				// adjust velocity to floor direction

				delta.y = 0;
				velocity.y = 0;

				if (delta.x != 0)
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

						if (input->run)
						{
							currentNode_ = 0;
						}
						else
						{
							const Collision::Node *nextNode;

							if (dest == &currentNode_->line.p1)
								nextNode = currentNode_->prev;
							else
								nextNode = currentNode_->next;

							if (nextNode != 0 && !nextNode->floor)
								nextNode = 0;

							bool isHullNode = nextNode >= &currentHull_.nodes[0] &&
								nextNode <= &currentHull_.nodes[2];

							if (nextNode != 0 && !isHullNode)
								currentHull_ = Collision::createHull(nextNode, bbox);

							currentNode_ = nextNode;
						}

						nextDelta.x = fpm::max(0, length - fpm::length(delta)) * fpm::sign(delta.x);
					}
				}
			}
			else if (currentNode_ != 0)
			{
				// wall/roof collision

				fixvec2 normal = fpm::normal(currentNode_->line);

				if (fpm::dot(velocity, normal) < 0)
				{
					direction = fpm::normalize(currentNode_->line.p2 - currentNode_->line.p1);
					fixed length = fpm::dot(direction, velocity);
					fixvec2 vel = direction * length;

					if (fpm::fabs(vel.y) > fpm::fabs(velocity.y))
						vel *= fpm::fabs(velocity.y / vel.y);

					if (fpm::dot(normal, fixvec2(0, -1)) <= 0)
						velocity = vel;

					delta = vel * dts;
				}
			}

			if (delta != fixvec2(0, 0))
			{
				Collision::Result collision = Collision::resolve(map, position, position + delta, bbox);
				position = collision.position;

				if (collision.node)
				{
					if (wasInFloor && !collision.hull.nodes[collision.iHullNode].floor)
					{
						velocity.x = 0;
						nextDelta.x = 0;

						// revert node in case we switched to the next one but collided
						// with a wall before actually reaching it
						if (currentNode_ != prevNode)
						{
							currentNode_ = prevNode;
							currentHull_ = prevHull;
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

						currentHull_ = collision.hull;
						currentNode_ = &currentHull_.nodes[collision.iHullNode];
					}
				}
				else if (currentNode_ != 0 && !currentNode_->floor)
				{
					currentNode_ = 0;
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

		ducked_ = false;
		currentNode_ = 0;
	}

	bool SoldierPhysics::ducking() const
	{
		return ducked_;
	}

	bool SoldierPhysics::floor() const
	{
		return currentNode_ != 0 && currentNode_->floor;
	}
}
