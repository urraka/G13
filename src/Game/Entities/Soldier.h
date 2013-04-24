#pragma once

#include "../Collision.h"
#include "../Components/SoldierPhysics.h"

class Soldier : public Entity
{
public:
	Soldier();

	enum MoveInput
	{
		MoveLeft = 0x01,
		MoveRight = 0x02,
		MoveUp = 0x04,
		MoveDown = 0x08
	};

	void update(Time dt);
	void draw(SpriteBatch *batch, float framePercent);
	void move(MoveInput moveInput);
	void spawn(vec2 pos);
	void moveTo(vec2 pos);
	void map(const Collision::Map *map);

private:
	enum MoveMode
	{
		MovingToTarget,
		FreeMovement
	};

	uint32_t moveInput_;
	Sprite sprite_;
	vec2 targetPosition_;
	MoveMode moveMode_;

	SoldierPhysics physics_;
};
