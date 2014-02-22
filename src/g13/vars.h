#pragma once

#include "math.h"

#ifdef G13_VARS_SOURCE
	#define constant(name, default_value) \
		namespace _ { fixed name = default_value; }
#else
	#define constant(name, default_value) \
		namespace _ { extern fixed name; } \
		static const fixed &name = _::name;
#endif

namespace g13
{
	namespace vars
	{
		constant(Gravity    , 1470);
		constant(JumpVel    , -700);
		constant(WalkVel    ,  500);
		constant(RunVel     ,  800);
		constant(AirMoveVel ,  500);
		constant(MoveAcc    , 1800);
		constant(AirMoveAcc ,  800);
		constant(BreakAcc   , 2200);
		constant(AirBreakAcc,  800);
		constant(LimitAcc   ,  800);
	}
}

#undef constant
