#include <Game/Game.h>
#include <Game/Entities/Character.h>

Character::Character() : moveInput_(0)
{
	position_[PreviousFrame] = position_[CurrentFrame] = vec2(0.0f, -100.0f);
	vec2 tex0(1.0f, 1.0f);
	vec2 tex1 = tex0 + vec2(52.0f, 82.0f);
	vec2 texSize(375.0f, 82.0f);

	sprite_.size = vec2(52.0f, 82.0f);
	sprite_.center = vec2(26.0f, 78.0f);
	sprite_.texcoords = vec4(vec2(tex0 / texSize), vec2(tex1 / texSize));
}

void Character::update(Time dt)
{
	const float kVelocity = 200.0f;

	float dts = Clock::toSeconds<float>(dt);

	vec2 &position = position_[CurrentFrame];
	position_[PreviousFrame] = position;

	velocity_.x = moveInput_ & MoveRight ? kVelocity : moveInput_ & MoveLeft ? -kVelocity : 0.0f;
	velocity_.y = moveInput_ & MoveDown ? kVelocity : moveInput_ & MoveUp ? -kVelocity : 0.0f;
	sprite_.scale.x = velocity_.x > 0.0f ? -1.0f : velocity_.x < 0.0f ? 1.0f : sprite_.scale.x;
	position += velocity_ * dts;
	moveInput_ = 0;
}

void Character::draw(SpriteBatch *batch, float framePercent)
{
	sprite_.position = glm::mix(position_[PreviousFrame], position_[CurrentFrame], framePercent);
	batch->add(sprite_);
}

void Character::move(MoveInput moveInput)
{
	moveInput_ |= moveInput;
}
