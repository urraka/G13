#pragma once

namespace hlp {

struct countof_check {
	class Is_pointer; // incomplete
	class Is_array {};
	template <typename T>
	static Is_pointer check_type(const T*, const T* const*);
	static Is_array check_type(const void*, const void*);
};

} // hlp

#define countof(arr) ( \
	0 * sizeof(reinterpret_cast<const ::hlp::countof_check*>(arr)) + \
	0 * sizeof(::hlp::countof_check::check_type((arr), &(arr))) + \
	sizeof(arr) / sizeof((arr)[0]) )
