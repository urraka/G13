#pragma once

#include "debug.h"
#include <sys/sys.h>

namespace g13 {

typedef sys::Time  Time;
typedef sys::Event Event;

#ifdef DEBUG
	extern Debugger *dbg;
#endif

struct Frame
{
	Time time;
	Time delta;
	float percent;
};

void initialize();
void display();
void terminate();

}
