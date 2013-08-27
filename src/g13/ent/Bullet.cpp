#include "Bullet.h"

namespace g13 {
namespace ent {

void Bullet::update(Time dt)
{
	if (state != Alive)
		return;

	if (physics.update(dt))
		state = Impact;

	graphics.update(dt, &physics);
}

void Bullet::spawn(const Collision::Map *map, fixvec2 position, fixed speed, fixed angle)
{
	state = Alive;

	physics.map = map;
	physics.position = position;
	physics.velocity.x = fpm::cos(angle) * speed;
	physics.velocity.y = fpm::sin(angle) * speed;

	graphics.position.set(from_fixed(position));
	graphics.angle.set(angle.to_float());
}

}} // g13::ent
