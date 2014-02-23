#if !defined(G13_VARS_HEADER) || defined(G13_VARS_SOURCE) || defined(G13_VARS_LOAD)

#include "math.h"

#if defined(G13_VARS_SOURCE)
	#define VARS_START()
	#define VARS_END()
	#define define_var(name, default_value) \
		namespace _ { fixed name = default_value; }
#elif defined(G13_VARS_LOAD)
	#include <hlp/ConfigFile.h>
	#define VARS_START() void load(const char *filename) { hlp::ConfigFile config(filename)
	#define VARS_END() }
	#define define_var(name, default_value) \
		_::name = fpm::from_string(config.readString(var_name(#name), #default_value))
#else
	#define G13_VARS_HEADER
	#define VARS_START()
	#define VARS_END() void load(const char *filename);
	#define define_var(name, default_value) \
		namespace _ { extern fixed name; } \
		static const fixed &name = _::name;
#endif

namespace g13
{
	namespace vars
	{
		VARS_START();

		define_var(Gravity      , 1470);
		define_var(JumpVel      , -700);
		define_var(WalkVel      ,  500);
		define_var(RunVel       ,  800);
		define_var(AirMoveVel   ,  500);
		define_var(MoveAcc      , 1800);
		define_var(AirMoveAcc   ,  800);
		define_var(BreakAcc     , 2200);
		define_var(AirBreakAcc  ,  800);
		define_var(LimitAcc     ,  800);
		define_var(MaxFloorSlope,    2);

		VARS_END();
	}
}

#undef define_var
#undef VARS_START
#undef VARS_END

#endif
