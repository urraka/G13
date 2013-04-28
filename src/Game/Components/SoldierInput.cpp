#include "../../System/Keyboard.h"
#include "SoldierInput.h"

#include <fstream>
#include <iostream>

SoldierInput::SoldierInput()
	:	left(false),
		right(false),
		jump(false),
		run(false),
		duck(false),
		time_(0),
		replaying_(false)
{
}

void SoldierInput::update(Time dt)
{
	if (replaying_)
	{
		if (replayIndex_ >= recording_.size())
		{
			replaying_ = false;
			time_ = 0;
			recording_.resize(0);
			reset();
			return;
		}

		const StateChange &change = recording_[replayIndex_];

		uint32_t tick = (uint32_t)(time_ / dt);

		if (change.tick == tick)
		{
			left  = change.state & 0x01;
			right = change.state & 0x02;
			jump  = change.state & 0x04;
			run   = change.state & 0x08;
			duck  = change.state & 0x10;

			replayIndex_++;
		}
	}
	else
	{
		SoldierInput prev = *this;

		reset();

		left  = Keyboard::pressed(Keyboard::Left);
		right = Keyboard::pressed(Keyboard::Right);
		jump  = Keyboard::pressed(Keyboard::Up);
		duck  = Keyboard::pressed(Keyboard::Down);

		if (left && right) left = right = false;

		// input recording

		if (prev != *this)
		{
			StateChange change;

			change.tick = (uint32_t)(time_ / dt);
			change.state = 0;
			change.state |= 0x01 * left;
			change.state |= 0x02 * right;
			change.state |= 0x04 * jump;
			change.state |= 0x08 * run;
			change.state |= 0x10 * duck;

			// if (recording_.size() == recording_.capacity())
			// 	recording_.reserve(recording_.capacity() + 1024);

			recording_.push_back(change);
		}
	}

	time_ += dt;
}

void SoldierInput::reset()
{
	left  = false;
	right = false;
	jump  = false;
	run   = false;
	duck  = false;
}

void SoldierInput::save(const char *filename)
{
	std::ofstream file(filename, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);

	if (!file.is_open())
		return;

	file.write(reinterpret_cast<const char*>(recording_.data()), recording_.size() * sizeof(StateChange));
}

void SoldierInput::replay(const char *filename)
{
	std::ifstream file(filename, std::ifstream::in | std::ifstream::binary | std::ifstream::ate);

	if (!file.is_open())
		return;

	reset();
	time_ = 0;
	replaying_ = true;
	replayIndex_ = 0;

	size_t size = file.tellg();
	recording_.resize(size / sizeof(StateChange));

	if (size > 0)
	{
		file.seekg(0, std::ifstream::beg);
		file.read(reinterpret_cast<char*>(&recording_[0]), size);
	}
}

bool SoldierInput::operator==(SoldierInput const & rhs)
{
	return left == rhs.left &&
		right == rhs.right &&
		jump  == rhs.jump &&
		run   == rhs.run &&
		duck  == rhs.duck;
}

bool SoldierInput::operator!=(SoldierInput const & rhs)
{
	return !(*this == rhs);
}
