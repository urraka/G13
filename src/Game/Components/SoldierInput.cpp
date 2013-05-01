#include "../../System/Keyboard.h"
#include "SoldierInput.h"

SoldierInput::SoldierInput()
	:	left(false),
		right(false),
		jump(false),
		run(false),
		duck(false)
		/*tick_(0),
		replaying_(false)*/
{
}

void SoldierInput::update()
{
	/*if (replaying_)
	{
		if (replayIndex_ >= recording_.size())
		{
			replaying_ = false;
			tick_ = 0;
			recording_.resize(0);
			reset();
			return;
		}

		const StateChange &change = recording_[replayIndex_];

		if (change.tick == tick_)
		{
			#ifdef DEBUG
				std::cout << "Replay tick: " << tick_ << std::endl;
			#endif

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

			change.tick = tick_;
			change.state = 0;
			change.state |= 0x01 * left;
			change.state |= 0x02 * right;
			change.state |= 0x04 * jump;
			change.state |= 0x08 * run;
			change.state |= 0x10 * duck;

			recording_.push_back(change);
		}
	}

	tick_++;*/

	reset();

	left  = Keyboard::pressed(Keyboard::Left);
	right = Keyboard::pressed(Keyboard::Right);
	jump  = Keyboard::pressed(Keyboard::Up);
	duck  = Keyboard::pressed(Keyboard::Down);

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

/*void SoldierInput::save(const char *filename)
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
	tick_ = 0;
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

uint32_t SoldierInput::tick() const
{
	return tick_;
}*/

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
