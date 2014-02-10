#include "Player.h"

namespace g13 {
namespace net {

Player::Player()
{
	soldier.entity.data = this;
	bullets.reserve(100);
}

void Player::initialize()
{
	state = Disconnected;
	health = MaxHealth;

	nickname.clear();
	color = gfx::Color(0);

	soldier.reset(fixvec2(0, 0));
	bullets.clear();

	kills = 0;
	deaths = 0;
}

void Player::updateBullets(Time dt, const coll::World &world)
{
	for (size_t i = 0; i < bullets.size(); i++)
	{
		bullets[i].update(dt, world);

		if (bullets[i].state == ent::Bullet::Dead)
		{
			std::swap(bullets[i--], bullets[bullets.size() - 1]);
			bullets.pop_back();
		}
	}
}

}} // g13::net
