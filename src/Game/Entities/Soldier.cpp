#include "../Game.h"
#include "../Replay.h"
#include "Soldier.h"

Soldier::Soldier()
{
	vec2 tex0(1.0f, 1.0f);
	vec2 tex1 = tex0 + vec2(52.0f, 82.0f);
	vec2 texSize(375.0f, 82.0f);

	sprite.size = vec2(52.0f, 82.0f);
	sprite.center = vec2(26.0f, 78.0f);
	sprite.texcoords = vec4(vec2(tex0 / texSize), vec2(tex1 / texSize));

	physics.bbox = fixrect(fixed(-17), fixed(-66), fixed(17), fixed(0));
	physics.input = &input;
}

void Soldier::update(Time dt, Replay *replay)
{
	vec2 &position = position_[CurrentFrame];
	position_[PreviousFrame] = position;

	if (replay->state() == Replay::Playing)
		input = replay->input();
	else
		input.update();

	physics.update(dt);

	position.x = physics.position.x.to_float();
	position.y = physics.position.y.to_float();

	sprite.scale.x = input.right ? -1.0f : input.left ? 1.0f : sprite.scale.x;
}

void Soldier::draw(SpriteBatch *batch, float framePercent)
{
	sprite.position = glm::mix(position_[PreviousFrame], position_[CurrentFrame], framePercent);
	batch->add(sprite);
}

void Soldier::reset(fixvec2 pos)
{
	position_[PreviousFrame].x = pos.x.to_float();
	position_[PreviousFrame].y = pos.y.to_float();
	position_[CurrentFrame].x  = pos.x.to_float();
	position_[CurrentFrame].y  = pos.y.to_float();

	physics.reset(pos);
}

void Soldier::map(const Collision::Map *map)
{
	physics.map = map;
}
