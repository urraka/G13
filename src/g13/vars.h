#if !defined(G13_VARS_HEADER) || \
	 defined(G13_VARS_SOURCE) || \
	 defined(G13_VARS_LOAD)   || \
	 defined(G13_VARS_STRUCT) || \
	 defined(G13_VARS_GET)    || \
	 defined(G13_VARS_SET)

#include "math.h"

#if defined(G13_VARS_SOURCE)
	#define VARS_START()
	#define VARS_END()
	#define define_var(name, default_value) \
		namespace _ { static fixed name = default_value; } \
		const fixed &name = _::name;
#elif defined(G13_VARS_LOAD)
	#include <hlp/ConfigFile.h>
	#define VARS_START() void load(const char *filename) { hlp::ConfigFile config(filename)
	#define VARS_END() }
	#define define_var(name, default_value) \
		_::name = fpm::from_string(config.readString(var_name(#name), #default_value))
#elif defined(G13_VARS_GET)
	#define VARS_START() void get(Vars &v) {
	#define VARS_END() }
	#define define_var(name, default_value) v.name = _::name;
#elif defined(G13_VARS_SET)
	#define VARS_START() void set(const Vars &v) {
	#define VARS_END() }
	#define define_var(name, default_value) _::name = v.name;
#elif defined(G13_VARS_STRUCT)
	#define VARS_START() struct Vars {
	#define VARS_END() };
	#define define_var(name, default_value) fixed name;
#else
	#define G13_VARS_HEADER
	#ifndef G13_VARS_STRUCT
		#define G13_VARS_STRUCT
		#include "vars.h"
		#undef G13_VARS_STRUCT
	#endif
	#define VARS_START()
	#define VARS_END() \
		void load(const char *filename); \
		void get(Vars &v); \
		void set(const Vars &v);
	#define define_var(name, default_value) \
		extern const fixed &name;
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
