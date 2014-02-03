#include "SoldierGraphics.h"
#include "SoldierState.h"

#include <json/json.h>
#include <hlp/read.h>
#include <hlp/countof.h>
#include <vector>
#include <iostream>

namespace g13 {
namespace cmp {

// -----------------------------------------------------------------------------
// Spritesheet
// -----------------------------------------------------------------------------

struct Spritesheet
{
	struct Sprite
	{
		Sprite() {}
		Sprite(const Json::Value &info, const Spritesheet *spritesheet);

		vec2 center;
		vec2 tx0, tx1;
		float width;
		float height;
	};

	Spritesheet();

	int width;
	int height;

	Sprite eye;
	Sprite head;
	Sprite body;
	Sprite bodyrun;
	Sprite legstand;
	Sprite weapon;
	std::vector<Sprite> legs;
	std::vector<Sprite> armsfront;
	std::vector<Sprite> armsback;
};

static const Spritesheet &get_spritesheet()
{
	static Spritesheet spritesheet;
	return spritesheet;
}

// -----------------------------------------------------------------------------
// SoldierGraphics
// -----------------------------------------------------------------------------

const vec2 SoldierGraphics::NoTarget;

SoldierGraphics::SoldierGraphics()
	:	angle_(0),
		prevAngle_(0),
		currAngle_(0),
		rightwards_(true),
		running_(false),
		air_(false),
		runningTime_(0.0f),
		speed_(0.0f),
		bodyColor_(0)
{
}

void SoldierGraphics::update(Time dt, const SoldierState &state)
{
	prevPosition_ = currPosition_;
	currPosition_ = from_fixed(state.position);

	prevAngle_ = currAngle_;
	currAngle_ = state.angle;

	speed_      = state.velocity.x.to_float();
	air_        = !state.floor;
	running_    = state.floor && state.velocity.x != 0;
	rightwards_ = state.rightwards;
}

void SoldierGraphics::frame(const Frame &frame, const vec2 &target)
{
	gfx::Sprite *body   = &sprites_[Body];
	gfx::Sprite *head   = &sprites_[Head];
	gfx::Sprite *weapon = &sprites_[Weapon];
	gfx::Sprite *eyes[2] = {&sprites_[Eye1], &sprites_[Eye2]};
	gfx::Sprite *arms[2] = {&sprites_[ArmFront], &sprites_[ArmBack]};
	gfx::Sprite *legs[2] = {&sprites_[Leg1], &sprites_[Leg2]};

	position_ = glm::mix(prevPosition_, currPosition_, frame.percent);
	angle_ = glm::mix(prevAngle_, currAngle_, frame.percent);

	const Spritesheet &spritesheet = get_spritesheet();

	const vec2 &worldpos = position_;
	const vec2 scale = vec2(0.15f);
	const int armsAngleInterval = 180 / (spritesheet.armsfront.size() - 1);

	vec2 bodyOffset = vec2(0.0f, -175.0f);
	vec2 headOffset = vec2(0.0f);
	vec2 legsOffset = vec2(0.0f);

	float legangle[2] = {0.0f, 0.0f};
	float legscale[2] = {1.0f, 1.0f};

	const Spritesheet::Sprite *leginfo[2] = {
		&spritesheet.legstand,
		&spritesheet.legstand
	};

	if (running_ || air_)
	{
		float offsety = 0.0f;

		bool backwards = false;

		if (&target != &NoTarget)
		{
			if (glm::sign(target.x - worldpos.x - bodyOffset.x) != glm::sign(speed_))
				backwards = true;
		}
		else
		{
			if ((rightwards_ ? 1.0f : -1.0f) != glm::sign(speed_))
				backwards = true;
		}

		const int nFrames = spritesheet.legs.size();

		const vec2 legkeys[] = {
			vec2(  70.0f, 110.0f       ),
			vec2(   0.0f, 110.0f * 0.8f),
			vec2( -70.0f, 110.0f       ),
			vec2(-100.0f, 110.0f * 0.4f),
			vec2(  50.0f, 110.0f * 0.7f)
		};

		if (air_)
			runningTime_ = 0.28f;

		const float N = (float)countof(legkeys);
		const float basekey = 3.5f;

		float t1 = basekey + runningTime_ * N;

		for (int i = 0; i < 2; i++, t1 += N / 2.0f)
		{
			t1 = glm::mod(t1, N);

			const int iFrame = (int)glm::floor(nFrames * glm::mod(t1 - basekey + N, N) / N) % nFrames;

			const float t2 = glm::mod(basekey + N * iFrame / nFrames, N);

			int a1 = glm::floor(t1);
			int b1 = glm::mod(glm::ceil(t1), N);

			int a2 = glm::floor(t2);
			int b2 = glm::mod(glm::ceil(t2), N);

			vec2 end1 = glm::mix(legkeys[a1], legkeys[b1], t1 - a1);
			vec2 end2 = glm::mix(legkeys[a2], legkeys[b2], t2 - a2);

			leginfo[i]  = &spritesheet.legs[iFrame];
			legscale[i] = glm::length(end1) / glm::length(end2);
			legangle[i] = glm::atan(end1.y, end1.x);

			if (t1 <= 2.0f)
				offsety = 110.0f - end1.y;
		}

		headOffset.x += 8.0f;
		legsOffset.x -= 12.0f;
		bodyOffset.y += offsety;

		const float frameTime = 28.0f / glm::abs(speed_);
		const float totalTime = N * frameTime;
		const float delta = sys::to_seconds(frame.delta) / totalTime;

		if (backwards)
		{
			runningTime_ -= delta;

			if (runningTime_ < 0.0f)
				runningTime_ += 1.0f;
		}
		else
		{
			runningTime_ += delta;

			if (runningTime_ > 1.0f)
				runningTime_ -= 1.0f;
		}
	}
	else
	{
		runningTime_ = 0.0f;
	}

	float angle = 0.0f; // weapon angle in degrees [-90, 90]

	if (&target != &NoTarget)
	{
		vec2 v = target - worldpos - bodyOffset * scale;
		angle = glm::atan(v.y, v.x) * (180.0f / M_PI);

		angle = angle < -90.0f ? -180.0f - angle :
		        angle >  90.0f ?  180.0f - angle :
		        angle;

		rightwards_ = v.x >= 0.0f;

		targetInfo_.rightwards = rightwards_;
		targetInfo_.angle = (uint16_t)(((angle + 90.0f) / 180.0f) * UINT16_MAX);
	}
	else
	{
		angle = (angle_ / (float)UINT16_MAX) * 180.0f - 90.0f;
	}

	int armIndex = glm::floor((angle + 90.0f) / armsAngleInterval);
	float fixedAngle = armIndex * armsAngleInterval - 90.0f; // degrees

	// colors

	{
		weapon->color = gfx::Color(0x4C, 0x4C, 0x4C);
		body->color   = bodyColor_;
		head->color   = gfx::Color(0xFF, 0xCC, 0x99);
	}

	// body

	const Spritesheet::Sprite &bodyInfo = running_ ? spritesheet.bodyrun : spritesheet.body;

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

		legs[i]->width    = leginfo[i]->width;
		legs[i]->height   = leginfo[i]->height;
		legs[i]->center   = leginfo[i]->center;
		legs[i]->tx0      = leginfo[i]->tx0;
		legs[i]->tx1      = leginfo[i]->tx1;
		legs[i]->rotation = legangle[i];
		legs[i]->scale.x  = legscale[i];
	}

	// weapon

	weapon->position = vec2(0.0f);
	weapon->width    = spritesheet.weapon.width;
	weapon->height   = spritesheet.weapon.height;
	weapon->center   = spritesheet.weapon.center;
	weapon->tx0      = spritesheet.weapon.tx0;
	weapon->tx1      = spritesheet.weapon.tx1;
	weapon->rotation = glm::radians(angle);

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

		const mat2d m1 = mat2d::rotate(glm::radians(angle));
		const mat2d m2 = mat2d::rotate(glm::radians(fixedAngle));

		const vec2 &offs = spritesheet.weapon.center;

		for (int i = 0; i < 2; i++)
		{
			vec2 end1 = m1 * (end[i] - offs + vec2(weapdist[0], 0.0f));
			vec2 end2 = m2 * (end[i] - offs + vec2(weapdist[1], 0.0f));

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
			arms[i]->rotation = glm::atan(end1.y, end1.x);
			arms[i]->scale.x  = length1 / length2;
		}
	}

	// general transform

	{
		mat2d transform = mat2d::translate(worldpos.x, worldpos.y);
		transform *= mat2d::scale(scale.x, scale.y);
		transform *= mat2d::translate(bodyOffset.x, bodyOffset.y);

		if (!rightwards_)
			transform *= mat2d::scale(-1.0f, 1.0f);

		for (size_t i = 0; i < countof(sprites_); i++)
			sprites_[i].transform = transform;
	}
}

const gfx::Sprite (&SoldierGraphics::sprites())[9]
{
	return sprites_;
}

// -----------------------------------------------------------------------------
// Spritesheet
// -----------------------------------------------------------------------------

Spritesheet::Spritesheet()
{
	Json::Value root;

	Json::Reader json(Json::Features::strictMode());
	json.parse(hlp::read("data/soldier.json"), root);

	width = root["width"].asInt();
	height = root["height"].asInt();

	eye      = Sprite(root["eye"], this);
	head     = Sprite(root["head"], this);
	body     = Sprite(root["body"], this);
	bodyrun  = Sprite(root["body-running"], this);
	legstand = Sprite(root["leg-standing"], this);
	weapon   = Sprite(root["weapon"], this);

	const Json::Value *list = &root["leg"];

	for (int i = 0; i < (int)list->size(); i++)
		legs.push_back(Sprite((*list)[i], this));

	list = &root["arm-front"];

	for (int i = 0; i < (int)list->size(); i++)
		armsfront.push_back(Sprite((*list)[i], this));

	list = &root["arm-back"];

	for (int i = 0; i < (int)list->size(); i++)
		armsback.push_back(Sprite((*list)[i], this));
}

Spritesheet::Sprite::Sprite(const Json::Value &info, const Spritesheet *spritesheet)
{
	float x = info["x"].asFloat();
	float y = info["y"].asFloat();

	width = info["width"].asFloat();
	height = info["height"].asFloat();

	center.x = info["cx"].asFloat();
	center.y = info["cy"].asFloat();

	tx0.x = x / spritesheet->width;
	tx0.y = y / spritesheet->height;
	tx1.x = (x + width) / spritesheet->width;
	tx1.y = (y + height) / spritesheet->height;
}

}} // g13::cmp
