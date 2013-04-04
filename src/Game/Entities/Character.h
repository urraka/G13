#pragma once

class Character : public Entity
{
public:
	Character();

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

private:
	uint32_t moveInput_;
	Sprite sprite_;
};
