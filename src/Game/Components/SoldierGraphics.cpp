#include "SoldierGraphics.h"

namespace cmp {

SoldierGraphics::AnimationData SoldierGraphics::data_;

SoldierGraphics::SoldierGraphics()
{
	animation.data(data_.frames, data_.animations);
	animation.set(Standing);
	updateSprite(animation.frame());
}

void SoldierGraphics::update(Time dt, const SoldierState &state)
{
	position.update();
	position.current = math::from_fixed(state.position);

	int notMoving = Standing;
	int moving = Walking;
	int jumping = Jumping;
	int falling = Falling;

	if (state.duck)
	{
		notMoving = Ducking;
		moving = DuckWalking;
		jumping = DuckJumping;
		falling = DuckFalling;
	}

	if (state.floor)
	{
		if (state.velocity.x != 0 && animation.id() != moving)
		{
			int frame = data_.animations[moving].first;

			if (animation.frameIndex() == frame)
				frame++;

			animation.set(moving, frame);
		}
		else if (state.velocity.x == 0)
			animation.set(notMoving);
	}
	else
	{
		animation.set(state.velocity.y <= 0 ? jumping : falling);
	}

	animation.update(dt);
	updateSprite(animation.frame());

	sprite.sx = state.flip ? -1.f : 1.0f;
}

void SoldierGraphics::frame(float percent)
{
	vec2 pos = position.value(percent);

	sprite.x = pos.x;
	sprite.y = pos.y;
}

void SoldierGraphics::updateSprite(const Frame *frame)
{
	sprite.width = (float)frame->width;
	sprite.height = (float)frame->height;

	sprite.cx = (float)frame->cx;
	sprite.cy = (float)frame->cy;

	vec2 tex0 = vec2((float)frame->x, (float)frame->y);
	vec2 tex1 = tex0 + vec2(sprite.width, sprite.height);
	vec2 texSize(375.0f, 82.0f);

	tex0 /= texSize;
	tex1 /= texSize;

	sprite.u[0] = tex0.x;
	sprite.v[0] = tex0.y;
	sprite.u[1] = tex1.x;
	sprite.v[1] = tex1.y;
}

// animation data

SoldierGraphics::AnimationData::AnimationData()
{
	int cx = 26;
	int cy = 78;
	int w = 54;
	int h = 82;

	frames[Standing00   ] = Frame(  0, 0, w, h, cx, cy, 0);
	frames[Ducking00    ] = Frame(212, 0, w, h, cx, cy, 0);
	frames[Walking00    ] = Frame( 53, 0, w, h, cx, cy, Clock::seconds(0.1));
	frames[Walking01    ] = Frame(106, 0, w, h, cx, cy, Clock::seconds(0.1));
	frames[DuckWalking00] = Frame(265, 0, w, h, cx, cy, Clock::seconds(0.1));
	frames[DuckWalking01] = Frame(318, 0, w, h, cx, cy, Clock::seconds(0.1));
	frames[Falling00    ] = Frame( 53, 0, w, h, cx, cy, 0);
	frames[DuckFalling00] = Frame(265, 0, w, h, cx, cy, 0);

	animations[Standing    ] = AnimationInfo(Standing00   , Standing00   );
	animations[Ducking     ] = AnimationInfo(Ducking00    , Ducking00    );
	animations[Walking     ] = AnimationInfo(Walking00    , Walking01    );
	animations[DuckWalking ] = AnimationInfo(DuckWalking00, DuckWalking01);
	animations[Jumping    ] = AnimationInfo(Walking01    , Walking01    );
	animations[Falling    ] = AnimationInfo(Walking00    , Walking00    );
	animations[DuckJumping] = AnimationInfo(DuckWalking01, DuckWalking01);
	animations[DuckFalling] = AnimationInfo(DuckWalking00, DuckWalking00);
}

} // cmp
