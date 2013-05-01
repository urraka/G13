#pragma once

#include <stdint.h>

class SoldierInput
{
public:
	SoldierInput();
	void update();
	void reset();
	uint8_t serialize() const;
	void unserialize(uint8_t data);

	bool left;
	bool right;
	bool jump;
	bool run;
	bool duck;
/*
private:
	struct StateChange
	{
		uint32_t tick;
		uint8_t state;
	};

	uint32_t tick_;
	std::vector<StateChange> recording_;
	bool replaying_;
	size_t replayIndex_;*/
};
