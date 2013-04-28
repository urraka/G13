#include "../Game.h"
#include "Soldier.h"

Soldier::Soldier()
{
	vec2 tex0(1.0f, 1.0f);
	vec2 tex1 = tex0 + vec2(52.0f, 82.0f);
	vec2 texSize(375.0f, 82.0f);

	sprite_.size = vec2(52.0f, 82.0f);
	sprite_.center = vec2(26.0f, 78.0f);
	sprite_.texcoords = vec4(vec2(tex0 / texSize), vec2(tex1 / texSize));

	physics_.bbox = fixrect(fixed(-17), fixed(-66), fixed(17), fixed(0));
	physics_.input = &input_;
}

void Soldier::update(Time dt)
{
	vec2 &position = position_[CurrentFrame];
	position_[PreviousFrame] = position;

	input_.update(dt);
	physics_.update(dt);

	position.x = physics_.position.x.to_float();
	position.y = physics_.position.y.to_float();

	sprite_.scale.x = input_.right ? -1.0f : input_.left ? 1.0f : sprite_.scale.x;
}

void Soldier::draw(SpriteBatch *batch, float framePercent)
{
	sprite_.position = glm::mix(position_[PreviousFrame], position_[CurrentFrame], framePercent);
	batch->add(sprite_);
}

void Soldier::spawn(vec2 pos)
{
	position_[PreviousFrame] = pos;
	position_[CurrentFrame] = pos;

	physics_.teleport(fixvec2(fixed(pos.x), fixed(pos.y)));
}

void Soldier::map(const Collision::Map *map)
{
	physics_.map = map;
}

void Soldier::saveInput(const char *filename)
{
	input_.save(filename);
}

void Soldier::replay(const char *filename)
{
	input_.replay(filename);
}
