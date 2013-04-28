#pragma once

#include <stdint.h>

typedef uint64_t Time;

class Clock
{
public:
	static Time time();

	template<typename T> static T toSeconds(Time time) { return T((double)time / 1000000.0); }
	template<typename T> static Time seconds(T value) { return Time(value * (T)1000000); }
	template<typename T> static Time milliseconds(T value) { return Time(value * (T)1000); }
};
