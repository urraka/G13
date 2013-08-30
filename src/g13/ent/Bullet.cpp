#include "Bullet.h"
#include <g13/cmp/BulletParams.h>

namespace g13 {
namespace ent {

Bullet::Bullet() {}

Bullet::Bullet(const cmp::BulletParams &params)
{
	state = Alive;
	id = params.playerid;

	physics.position = params.position;
	physics.velocity = fixvec2(fpm::cos(params.angle), fpm::sin(params.angle)) * params.speed;

	graphics.position.set(from_fixed(params.position));
	graphics.angle.set(params.angle.to_float());
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
