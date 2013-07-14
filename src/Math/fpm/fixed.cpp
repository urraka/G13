#include <fix16.h>
#include "fixed.h"

namespace math {
namespace fpm {

const fixed fixed::max     (fix16_maximum , true);
const fixed fixed::min     (fix16_minimum , true);
const fixed fixed::overflow(fix16_overflow, true);
const fixed fixed::one     (fix16_one     , true);
const fixed fixed::pi      (fix16_pi      , true);
const fixed fixed::e       (fix16_e       , true);
const fixed fixed::zero    (0             , true);

fixed fixed::from_value(int32_t value) { return fixed(value, true); }
int32_t fixed::value() const { return value_; }

fixed::fixed() {}
fixed::fixed(int32_t value, bool) : value_(value)    {}

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
