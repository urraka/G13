#pragma once

namespace Clock
{
	uint64_t time();

	template<typename T> inline T toSeconds(uint64_t time) { return T((double)time / 1000000); }
	template<typename T> inline uint64_t seconds(T value) { return uint64_t(value * (T)1000000); }
	template<typename T> inline uint64_t milliseconds(T value) { return uint64_t(value * (T)1000); }
}
