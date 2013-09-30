#pragma once

#include <stdint.h>
#include <iosfwd>

namespace math {
namespace fpm {
	class fixed;
}}

std::ostream& operator<<(std::ostream & stream, math::fpm::fixed const & n);

namespace math {
namespace fpm {

class fixed
{
private:
	int32_t value_;

public:
	static const fixed max;
	static const fixed min;
	static const fixed overflow;
	static const fixed one;
	static const fixed half;
	static const fixed pi;
	static const fixed e;
	static const fixed zero;

	static fixed from_value(int32_t value);
	int32_t value() const;

	fixed();
	fixed(int x);

	explicit fixed(float  x);
	explicit fixed(double x);

	int    to_int()    const;
	float  to_float()  const;
	double to_double() const;

	fixed& operator= (int rhs);

	bool operator==(int rhs) const;
	bool operator!=(int rhs) const;
	bool operator<=(int rhs) const;
	bool operator>=(int rhs) const;
	bool operator< (int rhs) const;
	bool operator> (int rhs) const;

	bool operator! ()                  const;
	bool operator==(fixed const & rhs) const;
	bool operator!=(fixed const & rhs) const;
	bool operator<=(fixed const & rhs) const;
	bool operator>=(fixed const & rhs) const;
	bool operator< (fixed const & rhs) const;
	bool operator> (fixed const & rhs) const;

	fixed operator- ()                  const;
	fixed operator++(int)               const;
	fixed operator--(int)               const;
	fixed operator+ (fixed const & rhs) const;
	fixed operator- (fixed const & rhs) const;
	fixed operator* (fixed const & rhs) const;
	fixed operator/ (fixed const & rhs) const;

	fixed& operator++();
	fixed& operator--();
	fixed& operator+=(fixed const & rhs);
	fixed& operator-=(fixed const & rhs);
	fixed& operator*=(fixed const & rhs);
	fixed& operator/=(fixed const & rhs);

	friend fixed sign (fixed const & x);
	friend fixed fabs (fixed const & x);
	friend fixed ceil (fixed const & x);
	friend fixed floor(fixed const & x);
	friend fixed sqrt (fixed const & x);
	friend fixed exp  (fixed const & x);
	friend fixed log  (fixed const & x);
	friend fixed sin  (fixed const & x);
	friend fixed cos  (fixed const & x);
	friend fixed tan  (fixed const & x);
	friend fixed asin (fixed const & x);
	friend fixed acos (fixed const & x);
	friend fixed atan (fixed const & x);
	friend fixed atan2(fixed const & x, fixed const & y);
	friend fixed min  (fixed const & x, fixed const & y);
	friend fixed max  (fixed const & x, fixed const & y);
	friend fixed fmod (fixed const & x, fixed const & y);

	friend std::ostream& (::operator<<)(std::ostream & stream, math::fpm::fixed const & n);
};

}} // math::fpm
