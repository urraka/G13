#include "SoldierGraphics.h"
#include "SoldierInput.h"
#include "SoldierPhysics.h"

SoldierGraphics::AnimationData SoldierGraphics::data_;

SoldierGraphics::AnimationData::AnimationData()
{
	int cx = 26;
	int cy = 78;

	frames[Standing00]    = Frame(0, 0, 54, 82, cx, cy, 0);
	frames[Ducking00]     = Frame(212, 0, 54, 82, cx, cy, 0);
	frames[Walking00]     = Frame(53, 0, 54, 82, cx, cy, Clock::seconds(0.1));
	frames[Walking01]     = Frame(106, 0, 54, 82, cx, cy, Clock::seconds(0.1));
	frames[DuckWalking00] = Frame(265, 0, 54, 82, cx, cy, Clock::seconds(0.1));
	frames[DuckWalking01] = Frame(318, 0, 54, 82, cx, cy, Clock::seconds(0.1));
	frames[Falling00]     = Frame(53, 0, 54, 82, cx, cy, 0);
	frames[DuckFalling00] = Frame(265, 0, 54, 82, cx, cy, 0);

	animations[Standing]    = AnimationInfo(Standing00, Standing00);
	animations[Ducking]     = AnimationInfo(Ducking00, Ducking00);
	animations[Walking]     = AnimationInfo(Walking00, Walking01);
	animations[DuckWalking] = AnimationInfo(DuckWalking00, DuckWalking01);
	animations[Falling]     = AnimationInfo(Falling00, Falling00);
	animations[DuckFalling] = AnimationInfo(DuckFalling00, DuckFalling00);
}

SoldierGraphics::SoldierGraphics()
{
	animation.data(data_.frames, data_.animations);
	animation.set(Standing);
	updateSprite(animation.frame());
}

void SoldierGraphics::update(Time dt)
{
	position.update();
	position.current = math::from_fixed(physics->position);

	int notMoving = Standing;
	int moving = Walking;
	int falling = Falling;

	if (physics->ducking())
	{
		notMoving = Ducking;
		moving = DuckWalking;
		falling = DuckFalling;
	}

	if (physics->floor())
	{
		if (physics->velocity.x != 0 && animation.id() != moving)
			animation.set(moving, data_.animations[moving].first + 1);
		else if (physics->velocity.x == 0)
			animation.set(notMoving);
	}
	else
	{
		animation.set(falling);
	}

	animation.update(dt);
	updateSprite(animation.frame());

	if (input->left)  sprite.scale.x = 1.f;
	if (input->right) sprite.scale.x = -1.f;
}

void SoldierGraphics::frame(float percent)
{
	sprite.position = position.value(percent);
}

void SoldierGraphics::updateSprite(const Frame *frame)
{
	sprite.size = vec2((float)frame->width, (float)frame->height);
	sprite.center = vec2((float)frame->cx, (float)frame->cy);

	vec2 tex0 = vec2((float)frame->x, (float)frame->y);
	vec2 tex1 = tex0 + sprite.size;
	vec2 texSize(375.0f, 82.0f);

	sprite.texcoords = vec4(vec2(tex0 / texSize), vec2(tex1 / texSize));
}
