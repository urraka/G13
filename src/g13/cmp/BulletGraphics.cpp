#include "BulletGraphics.h"
#include "BulletPhysics.h"
#include <gfx/gfx.h>

namespace g13 {
namespace cmp {

void BulletGraphics::update(Time dt, const BulletPhysics *physics)
{
	position.previous = position.current;
	angle.previous = angle.current;

	position.current = from_fixed(physics->position);

	vec2 delta = position.current - position.previous;
	angle.current = glm::atan(delta.y, delta.x);
}

void BulletGraphics::frame(Frame frame)
{
	position.interpolate(frame.percent);
	angle.interpolate(frame.percent);
}

gfx::Sprite BulletGraphics::sprite() const
{
	gfx::Sprite sprite;

	const vec2 texsize(128.0f, 32.0f);

	sprite.position = position;
	sprite.rotation = angle;
	sprite.center = vec2(84.0f, 4.0f);
	sprite.width = 90.0f;
	sprite.height = 9.0f;
	sprite.tx0 = vec2(16.0f, 11.0f) / texsize;
	sprite.tx1 = (vec2(16.0f, 11.0f) + vec2(90.0f, 9.0f)) / texsize;
	sprite.scale = vec2(0.5f, 0.25f);
	sprite.color = gfx::Color(0, 200);

	return sprite;
}

}} // g13::cmp
