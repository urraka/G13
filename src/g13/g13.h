#pragma once

#include <sys/sys.h>
#include <string>
#include <hlp/countof.h>

#include "types.h"
#include "debug.h"

namespace g13 {

typedef std::basic_string<uint32_t> string32_t;

typedef sys::Time  Time;
typedef sys::Event Event;

#ifdef DEBUG
	extern Debugger *dbg;
#endif

class Frame
{
public:
	Time time;
	Time delta;
	float percent;
};

void initialize();
void display();
void terminate();

}
