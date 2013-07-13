#include "SoldierInput.h"
#include "../Replay.h"

#include <sys/sys.h>

namespace cmp {

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

	left  = sys::pressed(sys::Left);
	right = sys::pressed(sys::Right);
	jump  = sys::pressed(sys::Up);
	run   = sys::pressed(sys::LeftShift) || sys::pressed(sys::RightShift);
	duck  = sys::pressed(sys::Down);

	if (left && right) left = right = false;
}

void SoldierInput::reset()
{
	left  = false;
	right = false;
	jump  = false;
	run   = false;
	duck  = false;
}

uint8_t SoldierInput::serialize() const
{
	uint8_t data = 0;
	data |= 0x01 * left;
	data |= 0x02 * right;
	data |= 0x04 * jump;
	data |= 0x08 * run;
	data |= 0x10 * duck;

	return data;
}

void SoldierInput::unserialize(uint8_t data)
{
	left  = data & 0x01;
	right = data & 0x02;
	jump  = data & 0x04;
	run   = data & 0x08;
	duck  = data & 0x10;
}

} // cmp
