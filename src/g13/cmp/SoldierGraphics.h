#pragma once

#include <g13/g13.h>
#include <g13/math.h>

#include <gfx/Color.h>
#include <gfx/Sprite.h>

namespace g13 {
namespace cmp {

class SoldierGraphics
{
public:
	SoldierGraphics();

	enum { Leg1 = 0, Leg2, Body, Head, Eye1, Eye2, ArmBack, Weapon, ArmFront, SpriteCount };

	struct TargetInfo
	{
		TargetInfo() : angle(0), rightwards(true) {}
		uint16_t angle;
		bool rightwards;
	};

	void update(Time dt, const SoldierState &state);
	void frame(const Frame &frame, const vec2 &target = NoTarget);

	void setBodyColor(const gfx::Color &color) { bodyColor_ = color; }

	const vec2 &position() const { return position_; }
	const TargetInfo &targetInfo() const { return targetInfo_; }

	const gfx::Sprite (&sprites())[SpriteCount] { return sprites_; }
	const gfx::Sprite (&ropeSprites())[2]       { return ropeSprites_; }

	bool hasRope() const { return hooked_; }

private:
	vec2 position_;
	vec2 prevPosition_;
	vec2 currPosition_;

	bool hooked_;
	vec2 hook_;
	vec2 prevHook_;
	vec2 currHook_;

	uint16_t angle_;
	uint16_t prevAngle_;
	uint16_t currAngle_;
	bool rightwards_;

	bool running_;
	bool air_;

	float runningTime_;
	float speed_;

	TargetInfo targetInfo_;

	gfx::Color bodyColor_;
	gfx::Sprite sprites_[SpriteCount];
	gfx::Sprite ropeSprites_[2];

	static const vec2 NoTarget;
};

}} // g13::cmp
