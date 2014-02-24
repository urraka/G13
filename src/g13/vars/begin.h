#if !defined(G13_VARS_HEADER) || \
	 defined(G13_VARS_SOURCE) || \
	 defined(G13_VARS_LOAD)   || \
	 defined(G13_VARS_STRUCT) || \
	 defined(G13_VARS_GET)    || \
	 defined(G13_VARS_SET)

	#include <g13/math.h>

	#if defined(G13_VARS_SOURCE)
		#define VARS_START()
		#define VARS_END()
		#define define(name, default_value) \
			namespace _ { static fixed name = default_value; } \
			const fixed &name = _::name;
	#elif defined(G13_VARS_LOAD)
		#include <hlp/ConfigFile.h>
		#define VARS_START() void load(const char *filename) { hlp::ConfigFile config(filename)
		#define VARS_END() }
		#define define(name, default_value) \
			_::name = fpm::from_string(config.readString(var_name(#name), #default_value))
	#elif defined(G13_VARS_GET)
		#define VARS_START() void get(Vars &v) {
		#define VARS_END() }
		#define define(name, default_value) v.name = _::name;
	#elif defined(G13_VARS_SET)
		#define VARS_START() void set(const Vars &v) {
		#define VARS_END() }
		#define define(name, default_value) _::name = v.name;
	#elif defined(G13_VARS_STRUCT)
		#define VARS_START() struct Vars {
		#define VARS_END() };
		#define define(name, default_value) fixed name;
	#else
		#define G13_VARS_HEADER
		#ifndef G13_VARS_STRUCT
			#define G13_VARS_STRUCT
			#include <g13/vars.h>
			#undef G13_VARS_STRUCT
		#endif
		#define VARS_START()
		#define VARS_END() \
			void load(const char *filename); \
			void get(Vars &v); \
			void set(const Vars &v);
		#define define(name, default_value) \
			extern const fixed &name;
	#endif
#else
	#define VARS_START()
	#define VARS_END()
	#define define(name, default_value)
#endif
