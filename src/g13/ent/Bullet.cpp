#include "Bullet.h"

namespace g13 {
namespace ent {

Bullet::Bullet() {}

Bullet::Bullet(const fixvec2 &position, const fixed &speed, const fixed &angle)
{
	state = Alive;

	physics.position = position;
	physics.velocity = fixvec2(fpm::cos(angle) * speed, fpm::sin(angle) * speed);

	graphics.position.set(from_fixed(position));
	graphics.angle.set(angle.to_float());
}

void Bullet::update(Time dt, const Collision::Map *map)
{
	if (state != Alive)
		return;

	if (physics.update(dt, map))
		state = Impact;

	// TODO: change this 20000 number to some map bounds
	if (fpm::fabs(physics.position.x) > 20000 || fpm::fabs(physics.position.y) > 20000)
		state = Dead;

	graphics.update(dt, &physics);
}

}} // g13::ent
