#pragma once

#include "../../System/Clock.h"

#include <stdint.h>
#include <cstdio>
#include <vector>

class SoldierInput
{
public:
	SoldierInput();
	void update(Time dt);
	void reset();
	void save(const char *filename);
	void replay(const char *filename);

	bool left;
	bool right;
	bool jump;
	bool run;
	bool duck;

private:
	struct StateChange
	{
		uint32_t tick;
		uint8_t state;
	};

	Time time_;
	std::vector<StateChange> recording_;
	bool replaying_;
	size_t replayIndex_;

	bool operator==(SoldierInput const & rhs);
	bool operator!=(SoldierInput const & rhs);
};
