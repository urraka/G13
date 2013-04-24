#pragma once

class SoldierInput
{
public:
	enum Direction { None, Left, Right };

	Direction move;
	bool jump;
	bool run;
	bool duck;

	SoldierInput() : move(None), jump(false), run(false), duck(false) {}
};
