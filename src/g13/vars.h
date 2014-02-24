#include <g13/vars/begin.h>

namespace g13
{
	namespace vars
	{
		VARS_START();

		define(Gravity      , 1470);
		define(JumpVel      , -700);
		define(WalkVel      ,  500);
		define(RunVel       ,  800);
		define(AirMoveVel   ,  500);
		define(MoveAcc      , 1800);
		define(AirMoveAcc   ,  800);
		define(BreakAcc     , 2200);
		define(AirBreakAcc  ,  800);
		define(LimitAcc     ,  800);
		define(MaxFloorSlope,    2);

		VARS_END();
	}
}

#include <g13/vars/end.h>
