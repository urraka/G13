#include "SoldierGraphics.h"
#include "SoldierState.h"

#include <glm/gtx/transform.hpp>
#include <json/json.h>
#include <hlp/read.h>
#include <hlp/countof.h>
#include <vector>
#include <iostream>

namespace g13 {
namespace cmp {

struct Spritesheet
{
	static int W;
	static int H;

	struct Sprite
	{
		Sprite() {}

		Sprite(const Json::Value &info)
		{
			float x = info["x"].asFloat();
			float y = info["y"].asFloat();

			width = info["width"].asFloat();
			height = info["height"].asFloat();

			center.x = info["cx"].asFloat();
			center.y = info["cy"].asFloat();

			tx0.x = x / W;
			tx0.y = y / H;
			tx1.x = (x + width) / W;
			tx1.y = (y + height) / H;
		}

		vec2 center;
		vec2 tx0, tx1;
		float width;
		float height;
	};

	Sprite eye;
	Sprite head;
	Sprite body;
	Sprite bodyrun;
	Sprite legstand;
	Sprite weapon;
	std::vector<Sprite> legs;
	std::vector<Sprite> armsfront;
	std::vector<Sprite> armsback;

	Spritesheet()
	{
		Json::Value root;

		Json::Reader json(Json::Features::strictMode());
		json.parse(hlp::read("data/soldier.json"), root);

		W = root["width"].asInt();
		H = root["height"].asInt();

		eye      = Sprite(root["eye"]);
		head     = Sprite(root["head"]);
		body     = Sprite(root["body"]);
		bodyrun  = Sprite(root["body-running"]);
		legstand = Sprite(root["leg-standing"]);
		weapon   = Sprite(root["weapon"]);

		const Json::Value *list = &root["leg"];

		for (size_t i = 0; i < list->size(); i++)
			legs.push_back(Sprite((*list)[i]));

		list = &root["arm-front"];

		for (size_t i = 0; i < list->size(); i++)
			armsfront.push_back(Sprite((*list)[i]));

		list = &root["arm-back"];

		for (size_t i = 0; i < list->size(); i++)
			armsback.push_back(Sprite((*list)[i]));
	}
};

int Spritesheet::W = 0;
int Spritesheet::H = 0;

static Spritesheet spritesheet;

SoldierGraphics::AnimationData SoldierGraphics::data_;

SoldierGraphics::SoldierGraphics()
	:	moving_(false),
		flip_(false)
{
	animation.data(data_.frames, data_.animations);
	animation.set(Standing);
	updateSprite(animation.frame());
}

void SoldierGraphics::update(Time dt, const SoldierState &state)
{
	position.previous = position.current;
	position.current = from_fixed(state.position);

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

	sprite.scale.x = state.flip ? -1.f : 1.0f;


	moving_ = state.floor && state.velocity.x != 0;
	flip_ = !state.flip;
}

void SoldierGraphics::frame(float percent)
{
	gfx::Sprite *body   = &sprites_[2];
	gfx::Sprite *head   = &sprites_[3];
	gfx::Sprite *weapon = &sprites_[7];
	gfx::Sprite *eyes[] = { &sprites_[4], &sprites_[5] };
	gfx::Sprite *arms[] = { &sprites_[8], &sprites_[6] };
	gfx::Sprite *legs[] = { &sprites_[0], &sprites_[1] };

	const vec2 worldpos = position.interpolate(percent);
	const vec2 scale = vec2(0.15f);
	const int armsAngleInterval = 180 / (spritesheet.armsfront.size() - 1);

	vec2 bodyOffset = vec2(0.0f, -175.0f);
	vec2 headOffset = vec2(0.0f);
	vec2 legsOffset = vec2(0.0f);

	if (moving_)
	{
		headOffset.x += 8.0f;
		legsOffset.x -= 12.0f;
	}

	// TODO: update bodyoffset according to leg frame before calculating angle

	float angle = 0.0f;       // weapon angle [-90, 90]
	float fixedAngle = 0.0f;  // weapon angle fixed to armsAngleInterval
	int   armIndex = 0;

	{
		vec2 v = target - worldpos - bodyOffset * scale;
		angle = glm::atan(v.y, v.x) * (180.0f / M_PI);

		angle = angle < -90.0f ? -180.0f - angle :
		        angle >  90.0f ?  180.0f - angle :
		        angle;

		armIndex = glm::floor((angle + 90.0f) / armsAngleInterval);
		fixedAngle = armIndex * armsAngleInterval - 90.0f;

		flip_ = v.x < 0.0f;
	}

	// colors

	{
		// weapon->color = gfx::Color(0x4C, 0x4C, 0x4C);
		// body->color   = gfx::Color(0, 0, 0xFF);
		// head->color   = gfx::Color(0xFF, 0xCC, 0x99);
	}

	// body

	const Spritesheet::Sprite &bodyInfo = moving_ ? spritesheet.bodyrun : spritesheet.body;

	body->position = vec2(0.0f);
	body->width    = bodyInfo.width;
	body->height   = bodyInfo.height;
	body->center   = bodyInfo.center;
	body->tx0      = bodyInfo.tx0;
	body->tx1      = bodyInfo.tx1;

	// head

	head->position = vec2(6.0f, -180.0f) + headOffset;
	head->width    = spritesheet.head.width;
	head->height   = spritesheet.head.height;
	head->center   = spritesheet.head.center;
	head->tx0      = spritesheet.head.tx0;
	head->tx1      = spritesheet.head.tx1;

	// eyes

	{
		const float miny = -290.0f;
		const float maxy = -90.0f;
		const float arcx = -30.0f;
		const float arcy = -185.0f;
		const float arcr = 180.0f;

		float y = miny + ((angle + 90.0f) / 180.0f) * (maxy - miny);
		float x = arcx + glm::sqrt(arcr * arcr - (y - arcy) * (y - arcy));

		eyes[0]->position = vec2(0.0f, 0.0f);
		eyes[1]->position = vec2(-74.0f, 12.0f);

		for (int i = 0; i < 2; i++)
		{
			eyes[i]->position += vec2(x, y) + headOffset;

			eyes[i]->width  = spritesheet.eye.width;
			eyes[i]->height = spritesheet.eye.height;
			eyes[i]->center = spritesheet.eye.center;
			eyes[i]->tx0    = spritesheet.eye.tx0;
			eyes[i]->tx1    = spritesheet.eye.tx1;
		}
	}

	// legs

	legs[0]->position = vec2(100.0f, 66.0f);
	legs[1]->position = vec2(-100.0f, 66.0f);

	for (int i = 0; i < 2; i++)
	{
		legs[i]->position += legsOffset;

		legs[i]->width  = spritesheet.legstand.width;
		legs[i]->height = spritesheet.legstand.height;
		legs[i]->center = spritesheet.legstand.center;
		legs[i]->tx0    = spritesheet.legstand.tx0;
		legs[i]->tx1    = spritesheet.legstand.tx1;
	}

	// weapon

	weapon->position = vec2(0.0f);
	weapon->width    = spritesheet.weapon.width;
	weapon->height   = spritesheet.weapon.height;
	weapon->center   = spritesheet.weapon.center;
	weapon->tx0      = spritesheet.weapon.tx0;
	weapon->tx1      = spritesheet.weapon.tx1;
	weapon->rotation = angle;

	float weapdist[2] = {0.0f, 0.0f};

	{
		float angles[2] = {angle, fixedAngle};

		for (int i = 0; i < 2; i++)
		{
			const float distances[] = {400.0f, 300.0f, 275.0f, 275.0f, 250.0f};
			const float theta = 180.0f / (countof(distances) - 1);
			const float index = (angles[i] + 90.0f) / theta;
			const int a = glm::floor(index);
			const int b = glm::ceil(index);
			const float percent = (angles[i] - (a * theta - 90)) / theta;

			weapdist[i] = glm::mix(distances[a], distances[b], percent);
		}

		weapon->center.x -= weapdist[0];
	}

	// arms

	{
		const vec2 start[] = {
			vec2(-108.0f, -30.0f),
			vec2(108.0f, -30.0f)
		};

		const vec2 end[] = {
			vec2(82.0f, 82.0f),
			vec2(232.0f, 71.0f)
		};

		const Spritesheet::Sprite *armsInfo[2] = {
			armsInfo[0] = &(spritesheet.armsfront[armIndex]),
			armsInfo[1] = &(spritesheet.armsback[armIndex])
		};

		const mat4 m1 = glm::rotate(angle, 0.0f, 0.0f, 1.0f);
		const mat4 m2 = glm::rotate(fixedAngle, 0.0f, 0.0f, 1.0f);

		const vec2 &offs = spritesheet.weapon.center;

		for (int i = 0; i < 2; i++)
		{
			vec2 end1 = vec2(m1 * glm::vec4(end[i] - offs + vec2(weapdist[0], 0.0f), 0.0f, 1.0f));
			vec2 end2 = vec2(m2 * glm::vec4(end[i] - offs + vec2(weapdist[1], 0.0f), 0.0f, 1.0f));

			end1 -= start[i];
			end2 -= start[i];

			float length1 = glm::length(end1);
			float length2 = glm::length(end2);

			arms[i]->position = start[i];
			arms[i]->width    = armsInfo[i]->width;
			arms[i]->height   = armsInfo[i]->height;
			arms[i]->center   = armsInfo[i]->center;
			arms[i]->tx0      = armsInfo[i]->tx0;
			arms[i]->tx1      = armsInfo[i]->tx1;
			arms[i]->rotation = glm::atan(end1.y, end1.x) * (180.0f / M_PI);
			arms[i]->scale.x  = length1 / length2;
		}
	}

	// general transform

	{
		mat4 transform = glm::translate(worldpos.x, worldpos.y, 0.0f);
		transform *= glm::scale(scale.x, scale.y, 1.0f);
		transform *= glm::translate(bodyOffset.x, bodyOffset.y, 1.0f);

		if (flip_)
			transform *= glm::scale(-1.0f, 1.0f, 1.0f);

		for (size_t i = 0; i < countof(sprites_); i++)
			sprites_[i].transform = transform;
	}
}

const gfx::Sprite (&SoldierGraphics::sprites())[9]
{
	return sprites_;
}

void SoldierGraphics::updateSprite(const Frame *frame)
{
	sprite.width = (float)frame->width;
	sprite.height = (float)frame->height;

	sprite.center.x = (float)frame->cx;
	sprite.center.y = (float)frame->cy;

	vec2 tx0 = vec2((float)frame->x, (float)frame->y);
	vec2 tx1 = tx0 + vec2(sprite.width, sprite.height);
	vec2 txSize(375.0f, 82.0f);

	sprite.tx0 = tx0 / txSize;
	sprite.tx1 = tx1 / txSize;
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
	frames[Walking00    ] = Frame( 53, 0, w, h, cx, cy, sys::time<sys::Seconds>(0.1));
	frames[Walking01    ] = Frame(106, 0, w, h, cx, cy, sys::time<sys::Seconds>(0.1));
	frames[DuckWalking00] = Frame(265, 0, w, h, cx, cy, sys::time<sys::Seconds>(0.1));
	frames[DuckWalking01] = Frame(318, 0, w, h, cx, cy, sys::time<sys::Seconds>(0.1));
	frames[Falling00    ] = Frame( 53, 0, w, h, cx, cy, 0);
	frames[DuckFalling00] = Frame(265, 0, w, h, cx, cy, 0);

	animations[Standing    ] = AnimationInfo(Standing00   , Standing00   );
	animations[Ducking     ] = AnimationInfo(Ducking00    , Ducking00    );
	animations[Walking     ] = AnimationInfo(Walking00    , Walking01    );
	animations[DuckWalking ] = AnimationInfo(DuckWalking00, DuckWalking01);
	animations[Jumping     ] = AnimationInfo(Walking01    , Walking01    );
	animations[Falling     ] = AnimationInfo(Walking00    , Walking00    );
	animations[DuckJumping ] = AnimationInfo(DuckWalking01, DuckWalking01);
	animations[DuckFalling ] = AnimationInfo(DuckWalking00, DuckWalking00);
}

}} // g13::cmp
