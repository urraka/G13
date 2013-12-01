#include "BulletGraphics.h"
#include "BulletPhysics.h"
#include <gfx/gfx.h>

namespace g13 {
namespace cmp {

void BulletGraphics::update(Time dt, const BulletPhysics *physics)
{
	position.previous = position.current;
	velocity.previous = velocity.current;
	angle.previous = angle.current;

	position.current = from_fixed(physics->position);
	velocity.current = glm::length(from_fixed(physics->velocity));

	vec2 delta = position.current - position.previous;
	angle.current = glm::atan(delta.y, delta.x);
}

void BulletGraphics::frame(Frame frame)
{
	position.interpolate(frame.percent);
	velocity.interpolate(frame.percent);
	angle.interpolate(frame.percent);
}

gfx::Sprite BulletGraphics::sprite() const
{
	gfx::Sprite sprite;

	const vec2 texsize(128.0f, 32.0f);

	float w = 90.0f;
	float sx = ((float)velocity / 2000.0f);
	float dist = glm::length(position.get() - initialPosition);

	if (dist < sx * w)
		sx = dist / w;

	sprite.position = position;
	sprite.rotation = angle;
	sprite.center = vec2(86.0f, 4.0f);
	sprite.width = w;
	sprite.height = 9.0f;
	sprite.tx0 = vec2(16.0f, 11.0f) / texsize;
	sprite.tx1 = (vec2(16.0f, 11.0f) + vec2(90.0f, 9.0f)) / texsize;
	sprite.scale = vec2(sx, 0.35f);
	sprite.color = gfx::Color(0x00, 160);

	return sprite;
}

}} // g13::cmp
