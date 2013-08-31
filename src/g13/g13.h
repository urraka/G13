#pragma once

#include <sys/sys.h> // TODO: change this with some <sys/fwd.h>
#include "types.h"
#include "debug.h"

namespace g13 {

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
