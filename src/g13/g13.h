#pragma once

#include "debug.h"
#include <sys/sys.h>

namespace g13 {

typedef sys::Time  Time;
typedef sys::Event Event;

extern Debugger *dbg;

void initialize();
void display();
void terminate();

}
