#include "SoldierInput.h"

#include <sys/sys.h>

namespace g13 {
namespace cmp {

SoldierInput::SoldierInput()
	:	left(false),
		right(false),
		realLeft(false),
		realRight(false),
		jump(false),
		run(false),
		duck(false),
		shoot(false)
{
}

void SoldierInput::onKeyPress(const sys::Event::KeyEvent &event)
{
	switch (event.code)
	{
		case 'A': left  = realLeft  = true; right = false; break;
		case 'D': right = realRight = true; left  = false; break;
		case 'S': duck  = true; break;
		case 'W': jump  = true; break;

		case sys::LeftShift:
		case sys::RightShift:
			run = true;
			break;
	}
}

void SoldierInput::onKeyRelease(const sys::Event::KeyEvent &event)
{
	switch (event.code)
	{
		case 'A': left  = realLeft  = false; right = realRight; break;
		case 'D': right = realRight = false; left  = realLeft;  break;
		case 'S': duck  = false; break;
		case 'W': jump  = false; break;

		case sys::LeftShift:
		case sys::RightShift:
			run = false;
			break;
	}
}

void SoldierInput::onMousePress(const sys::Event::MouseButtonEvent &event)
{
	switch (event.code)
	{
		case sys::MouseLeft: shoot = true; break;
	}
}

void SoldierInput::onMouseRelease(const sys::Event::MouseButtonEvent &event)
{
	switch (event.code)
	{
		case sys::MouseLeft: shoot = false; break;
	}
}

}} // g13::cmp
