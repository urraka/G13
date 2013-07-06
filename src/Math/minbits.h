#pragma once

#include <stdint.h>
#include <stddef.h>

#define MINBITS(N) (math::minbits<N>::value)

namespace math {

// Minimum number of bits required to represent a given number as a compile time constant.

template<uint64_t N, size_t i = 1> struct minbits
{
	static const size_t value = N < (1ull <<  i) ?  i : minbits<N, i + 1>::value;
};

template<uint64_t N> struct minbits<N, 64>
{
	static const size_t value = 64;
};

} // math
