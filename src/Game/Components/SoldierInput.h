#pragma once

class SoldierInput
{
public:
	SoldierInput();
	void update();
	void reset();

	bool left;
	bool right;
	bool jump;
	bool run;
	bool duck;
};
