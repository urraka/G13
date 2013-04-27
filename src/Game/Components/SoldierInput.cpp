#include "../../System/Keyboard.h"
#include "SoldierInput.h"

SoldierInput::SoldierInput()
	:	left(false),
		right(false),
		jump(false),
		run(false),
		duck(false)
{
}

void SoldierInput::update()
{
	reset();

	if (Keyboard::pressed(Keyboard::Left))
		left = true;

	if (Keyboard::pressed(Keyboard::Right))
		right = true;

	if (Keyboard::pressed(Keyboard::Up))
		jump = true;

	if (Keyboard::pressed(Keyboard::Down))
		duck = true;
}

void SoldierInput::reset()
{
	left  = false;
	right = false;
	jump  = false;
	run   = false;
	duck  = false;
}
