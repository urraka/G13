#pragma once

#include <stdint.h>

class Replay;

class SoldierInput
{
public:
	SoldierInput();
	void update(Replay *replay);
	void reset();
	uint8_t serialize() const;
	void unserialize(uint8_t data);

	bool left;
	bool right;
	bool jump;
	bool run;
	bool duck;
};
