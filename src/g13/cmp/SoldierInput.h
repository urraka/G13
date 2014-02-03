#pragma once

#include <g13/g13.h>
#include <stdint.h>
#include <stddef.h>

namespace g13 {
namespace cmp {

class SoldierInput
{
public:
	SoldierInput();

	void onEvent(const sys::Event &event);
	void onKeyPress(const sys::Event::KeyEvent &event);
	void onKeyRelease(const sys::Event::KeyEvent &event);
	void onMousePress(const sys::Event::MouseButtonEvent &event);
	void onMouseRelease(const sys::Event::MouseButtonEvent &event);
	void onMouseMove(const sys::Event::MouseMoveEvent &event);

	void updateTargetAngle(const ent::Soldier &soldier);

	uint16_t angle;
	bool rightwards;
	bool left;
	bool right;
	bool realLeft;
	bool realRight;
	bool jump;
	bool run;
	bool duck;
	bool shoot;

	float mousex;
	float mousey;
};

}} // g13::cmp
