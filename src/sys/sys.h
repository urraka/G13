#pragma once

#include "platform.h"
#include "mouse.h"
#include "keyboard.h"
#include "Event.h"
#include <stdint.h>

namespace sys {

// enums
enum CallbackType   { Initialize, Display, Terminate };
enum TimeConversion { Seconds = 1000000, Milliseconds = 1000 };
enum CursorMode     { Normal, Hidden, Disabled };

// typedefs
typedef uint64_t Time;
typedef void (*Callback)();
typedef void* Cursor;

// general
void callback(CallbackType type, Callback callback);
int  run(int argc, char *argv[]);
void initialize();
void exit();

// screen/window
void fullscreen(bool enable);
void vsync(int interval);
void samples(int samples);
int  window_rotation();
void window_title(const char *title);
void window_size(int *width, int *height);
int  window_width();
int  window_height();
void framebuffer_size(int *width, int *height);
int  framebuffer_width();
int  framebuffer_height();
void window_size(int width, int height);
void window_size(float width, float height);
void window_position(int x, int y);
void window_position(float x, float y);

// events
Event *poll_events();

// input
bool   pressed(int code);
void   mouse(double *x, double *y);
double mousex();
double mousey();

// cursor
Cursor create_cursor(int width, int height, int cx, int cy, const void *data);
void   set_cursor(Cursor cursor);
void   cursor_mode(CursorMode mode);

// time
Time time();
double to_seconds(Time value);
double to_milliseconds(Time value);

template<TimeConversion C, typename T> Time time(T value)
{
	return (Time)(value * T(C));
}

} // sys
