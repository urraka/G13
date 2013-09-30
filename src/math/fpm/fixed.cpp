#include <fix16.h>
#include <ostream>
#include "fixed.h"

namespace math {
namespace fpm {

const fixed fixed::max      = fixed::from_value(fix16_maximum);
const fixed fixed::min      = fixed::from_value(fix16_minimum);
const fixed fixed::overflow = fixed::from_value(fix16_overflow);
const fixed fixed::one      = fixed::from_value(fix16_one);
const fixed fixed::half     = fixed::from_value(fix16_one / 2);
const fixed fixed::pi       = fixed::from_value(fix16_pi);
const fixed fixed::e        = fixed::from_value(fix16_e);
const fixed fixed::zero     = fixed::from_value(0);

fixed fixed::from_value(int32_t value) { fixed result; result.value_ = value; return result; }
int32_t fixed::value() const { return value_; }

fixed::fixed() {}

fixed::fixed(int    x) : value_(  fix16_from_int(x)   ) {}
fixed::fixed(float  x) : value_(  fix16_from_float(x) ) {}
fixed::fixed(double x) : value_(  fix16_from_dbl(x)   ) {}

int    fixed::to_int()    const { return fix16_to_int  (value_); }
float  fixed::to_float()  const { return fix16_to_float(value_); }
double fixed::to_double() const { return fix16_to_dbl  (value_); }

fixed& fixed::operator= (int rhs) { value_ = fix16_from_int(rhs); return *this; }

bool fixed::operator==(int rhs) const { return *this == fixed(rhs); }
bool fixed::operator!=(int rhs) const { return *this != fixed(rhs); }
bool fixed::operator<=(int rhs) const { return *this <= fixed(rhs); }
bool fixed::operator>=(int rhs) const { return *this >= fixed(rhs); }
bool fixed::operator< (int rhs) const { return *this <  fixed(rhs); }
bool fixed::operator> (int rhs) const { return *this >  fixed(rhs); }

bool fixed::operator! ()                  const { return value_ == 0;          }
bool fixed::operator==(fixed const & rhs) const { return value_ == rhs.value_; }
bool fixed::operator!=(fixed const & rhs) const { return value_ != rhs.value_; }
bool fixed::operator<=(fixed const & rhs) const { return value_ <= rhs.value_; }
bool fixed::operator>=(fixed const & rhs) const { return value_ >= rhs.value_; }
bool fixed::operator< (fixed const & rhs) const { return value_ <  rhs.value_; }
bool fixed::operator> (fixed const & rhs) const { return value_ >  rhs.value_; }

fixed fixed::operator- ()                  const { return from_value(-value_);                       }
fixed fixed::operator++(int)               const { return from_value(fix16_add(value_, fixed::one.value_)); }
fixed fixed::operator--(int)               const { return from_value(fix16_sub(value_, fixed::one.value_)); }
fixed fixed::operator+ (fixed const & rhs) const { return from_value(fix16_add(value_, rhs.value_));        }
fixed fixed::operator- (fixed const & rhs) const { return from_value(fix16_sub(value_, rhs.value_));        }
fixed fixed::operator* (fixed const & rhs) const { return from_value(fix16_mul(value_, rhs.value_));        }
fixed fixed::operator/ (fixed const & rhs) const { return from_value(fix16_div(value_, rhs.value_));        }

fixed& fixed::operator++()                  { value_ = fix16_add(value_, fixed::one.value_); return *this; }
fixed& fixed::operator--()                  { value_ = fix16_sub(value_, fixed::one.value_); return *this; }
fixed& fixed::operator+=(fixed const & rhs) { value_ = fix16_add(value_, rhs.value_);        return *this; }
fixed& fixed::operator-=(fixed const & rhs) { value_ = fix16_sub(value_, rhs.value_);        return *this; }
fixed& fixed::operator*=(fixed const & rhs) { value_ = fix16_mul(value_, rhs.value_);        return *this; }
fixed& fixed::operator/=(fixed const & rhs) { value_ = fix16_div(value_, rhs.value_);        return *this; }

}} // math::fpm

std::ostream& operator<<(std::ostream & stream, math::fpm::fixed const & n)
{
	char buf[13];
	fix16_to_str(n.value_, buf, std::min(5, (int)stream.precision()));
	stream << buf;
	return stream;
}
