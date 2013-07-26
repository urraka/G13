#pragma once

#include "utf8/unchecked.h"
#include <cstring>

namespace hlp {

inline bool utf8_valid(const char *str)
{
	return utf8::is_valid(str, str + strlen(str));
}

template<size_t N> void utf8_encode(const std::basic_string<uint32_t> &str, char (&dest)[N])
{
	assert(str.size() * 4 <= N - 1);

	char *result = utf8::unchecked::utf32to8(str.begin(), str.end(), dest);

	while (size_t(result - dest) < N)
		*(result++) = 0;
}

inline std::basic_string<uint32_t> utf8_decode(const char *str)
{
	std::basic_string<uint32_t> result;
	utf8::unchecked::utf8to32(str, str + strlen(str), std::back_inserter(result));
	return result;
}

} // hlp
