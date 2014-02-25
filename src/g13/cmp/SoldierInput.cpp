#include "SoldierInput.h"
#include <g13/ent/Soldier.h>

namespace g13 {
namespace cmp {

SoldierInput::SoldierInput()
	:	angle(0),
		rightwards(true),
		left(false),
		right(false),
		realLeft(false),
		realRight(false),
		jump(false),
		run(false),
		duck(false),
		shoot(false),
		rope(false)
{
}

void SoldierInput::onEvent(const sys::Event &event)
{
	switch (event.type)
	{
		case sys::KeyPress:           onKeyPress(event.key);             break;
		case sys::KeyRelease:         onKeyRelease(event.key);           break;
		case sys::MouseButtonPress:   onMousePress(event.mouseButton);   break;
		case sys::MouseButtonRelease: onMouseRelease(event.mouseButton); break;
		case sys::MouseMove:          onMouseMove(event.mouseMove);      break;

		default: break;
	}
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
		case sys::MouseRight: rope = true; break;
	}
}

void SoldierInput::onMouseRelease(const sys::Event::MouseButtonEvent &event)
{
	switch (event.code)
	{
		case sys::MouseLeft: shoot = false; break;
		case sys::MouseRight: rope = false; break;
	}
}

void SoldierInput::onMouseMove(const sys::Event::MouseMoveEvent &event)
{
	mousex = event.x;
	mousey = event.y;
}

void SoldierInput::updateTargetAngle(const ent::Soldier &soldier)
{
	const SoldierGraphics::TargetInfo &info = soldier.graphics.targetInfo();

	angle = info.angle;
	rightwards = info.rightwards;
}

fixed SoldierInput::computeAngle() const
{
	const fixed value = fpm::from_value((int32_t)angle);
	const fixed maxValue = fpm::from_value(UINT16_MAX);

	fixed result = fpm::Pi * value / maxValue - fpm::Pi / fixed(2);

	if (!rightwards)
		result = -result + fpm::Pi;

	return result;
}

}} // g13::cmp
