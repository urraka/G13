#pragma once

#include <sys/Event.h>
#include <stdint.h>
#include <stddef.h>

namespace g13 {
namespace cmp {

class SoldierInput
{
public:
	SoldierInput();

	void onKeyPress(const sys::Event::KeyEvent &event);
	void onKeyRelease(const sys::Event::KeyEvent &event);
	void onMousePress(const sys::Event::MouseButtonEvent &event);
	void onMouseRelease(const sys::Event::MouseButtonEvent &event);

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
};

}} // g13::cmp
