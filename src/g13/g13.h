#pragma once

#include "debug.h"
#include <sys/sys.h>

namespace g13 {

typedef sys::Time          Time;
typedef sys::Event         Event;
typedef sys::ResizeEvent   ResizeEvent;
typedef sys::CharEvent     CharEvent;
typedef sys::MouseEvent    MouseEvent;
typedef sys::KeyboardEvent KeyboardEvent;

extern Debugger *dbg;

void initialize();
void display();
void terminate();

}
