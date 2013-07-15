#pragma once

#include "platform.h"
#include "mouse.h"
#include "keyboard.h"
#include "events.h"
#include <stdint.h>

namespace sys {

// enums
enum CallbackType   { Initialize, Display, Terminate };
enum TimeConversion { Seconds = 1000000, Milliseconds = 1000 };

// typedefs
typedef uint64_t Time;
typedef void (*Callback)();

// general
void callback(CallbackType type, Callback callback);
int  run(int argc, char *argv[]);
void initialize();
void exit();
bool exiting();

// screen/window
void fullscreen(bool enable);
void vsync(int interval);
void samples(int samples);
int  window_rotation();
void window_title(const char *title);
void window_size(int *width, int *height);
void framebuffer_size(int *width, int *height);
void window_size(int width, int height);
void window_size(float width, float height);
void window_position(int x, int y);
void window_position(float x, float y);

// events
Event *poll_events();

// input
bool pressed(int code);
void mouse(int *x, int *y);

// time
Time time();
double to_seconds(Time value);
double to_milliseconds(Time value);

template<TimeConversion C, typename T> Time time(T value)
{
	return (Time)(value * T(C));
}

} // sys
