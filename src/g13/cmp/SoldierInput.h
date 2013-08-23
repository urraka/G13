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

	uint16_t angle;
	bool rightwards;
	bool left;
	bool right;
	bool realLeft;
	bool realRight;
	bool jump;
	bool run;
	bool duck;
};

}} // g13::cmp
