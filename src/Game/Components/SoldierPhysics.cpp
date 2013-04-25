#include "../Game.h"
#include "SoldierPhysics.h"

SoldierPhysics::SoldierPhysics() : map(0), ducking_(false), currentNode_(0) {}

void SoldierPhysics::update(Time dt)
{
	const fixed dts = fixed((int)dt / 1000) / fixed(1000);
	const fixed G = fixed(1470); // 9.8 * 150
	const fixed walkVelocity = fixed(250);
	const fixed duckVelocity = fixed(150);
	const fixed runVelocity = fixed(300);

	acceleration.x = fixed::zero;
	acceleration.y = G;
	velocity.x = input.move == SoldierInput::Right ? walkVelocity : input.move == SoldierInput::Left ? -walkVelocity : fixed::zero;
	velocity.y += acceleration.y * dts;

	fixvec2 delta;

	if (currentNode_ != 0 && currentNode_->floor)
	{
		// adjust velocity to floor direction

		delta.x = velocity.x * dts;
		delta.y = fixed::zero;
		velocity.y = fixed::zero;

		if (delta.x != fixed::zero)
		{
			fixvec2 direction = currentNode_->line.p2 - currentNode_->line.p1;
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
				// rDelta.x -= fpm::max(fixed::zero, length - fpm::length(delta)) * fpm::sign(rDelta.x);
			}
		}
	}
	else if (currentNode_ != 0)
	{
		// wall/roof collision

		fixvec2 normal = fpm::normal(currentNode_->line);

		if (fpm::dot(velocity, normal) < fixed::zero)
		{
			fixvec2 direction = fpm::normalize(currentNode_->line.p2 - currentNode_->line.p1);
			fixed length = fpm::dot(direction, velocity);
			fixvec2 vel = direction * length;

			if (fpm::fabs(vel.y) > fpm::fabs(velocity.y))
				vel *= fpm::fabs(velocity.y / vel.y);

			if (fpm::dot(normal, fixvec2(fixed::zero, -fixed::one)) <= fixed::zero)
				velocity = vel;

			delta = vel * dts;
		}
	}
	else
	{
		delta = velocity * dts;
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
			}
			else
			{
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
