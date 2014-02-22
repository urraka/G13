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

	bool operator! ()                  const;
	bool operator==(fixed const & rhs) const;
	bool operator!=(fixed const & rhs) const;
	bool operator<=(fixed const & rhs) const;
	bool operator>=(fixed const & rhs) const;
	bool operator< (fixed const & rhs) const;
	bool operator> (fixed const & rhs) const;

	fixed operator- ()                  const;
	fixed operator+ (fixed const & rhs) const;
	fixed operator- (fixed const & rhs) const;
	fixed operator* (fixed const & rhs) const;
	fixed operator/ (fixed const & rhs) const;

	fixed  operator++(int);
	fixed  operator--(int);
	fixed& operator++();
	fixed& operator--();
	fixed& operator+=(fixed const & rhs);
	fixed& operator-=(fixed const & rhs);
	fixed& operator*=(fixed const & rhs);
	fixed& operator/=(fixed const & rhs);
};

}} // math::fpm
