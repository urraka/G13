#pragma once

#include <stdint.h>

namespace math {
namespace fpm {

class fixed
{
public:
	int32_t value_;

	fixed();
	fixed(int x);

	explicit fixed(float  x);
	explicit fixed(double x);

	fixed& operator= (int rhs);

	bool operator==(int rhs) const;
	bool operator!=(int rhs) const;
	bool operator<=(int rhs) const;
	bool operator>=(int rhs) const;
	bool operator< (int rhs) const;
	bool operator> (int rhs) const;

	bool operator! ()                 const;
	bool operator==(const fixed &rhs) const;
	bool operator!=(const fixed &rhs) const;
	bool operator<=(const fixed &rhs) const;
	bool operator>=(const fixed &rhs) const;
	bool operator< (const fixed &rhs) const;
	bool operator> (const fixed &rhs) const;

	fixed operator-()                 const;
	fixed operator+(const fixed &rhs) const;
	fixed operator-(const fixed &rhs) const;
	fixed operator*(const fixed &rhs) const;
	fixed operator/(const fixed &rhs) const;

	fixed  operator++(int);
	fixed  operator--(int);
	fixed& operator++();
	fixed& operator--();
	fixed& operator+=(const fixed &rhs);
	fixed& operator-=(const fixed &rhs);
	fixed& operator*=(const fixed &rhs);
	fixed& operator/=(const fixed &rhs);
};

}} // math::fpm

inline bool operator==(int lhs, const math::fpm::fixed &rhs) { return math::fpm::fixed(lhs) == rhs; }
inline bool operator!=(int lhs, const math::fpm::fixed &rhs) { return math::fpm::fixed(lhs) != rhs; }
inline bool operator<=(int lhs, const math::fpm::fixed &rhs) { return math::fpm::fixed(lhs) <= rhs; }
inline bool operator>=(int lhs, const math::fpm::fixed &rhs) { return math::fpm::fixed(lhs) >= rhs; }
inline bool operator< (int lhs, const math::fpm::fixed &rhs) { return math::fpm::fixed(lhs) <  rhs; }
inline bool operator> (int lhs, const math::fpm::fixed &rhs) { return math::fpm::fixed(lhs) >  rhs; }

inline math::fpm::fixed operator+(int lhs, const math::fpm::fixed &rhs) { return math::fpm::fixed(lhs) + rhs; }
inline math::fpm::fixed operator-(int lhs, const math::fpm::fixed &rhs) { return math::fpm::fixed(lhs) - rhs; }
inline math::fpm::fixed operator*(int lhs, const math::fpm::fixed &rhs) { return math::fpm::fixed(lhs) * rhs; }
inline math::fpm::fixed operator/(int lhs, const math::fpm::fixed &rhs) { return math::fpm::fixed(lhs) / rhs; }
