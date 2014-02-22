#include <fix16.h>
#include "fixed.h"
#include "functions.h"
#include "constants.h"

namespace math {
namespace fpm {

fixed::fixed() {}

fixed::fixed(int    x) : value_(  fix16_from_int(x)   ) {}
fixed::fixed(float  x) : value_(  fix16_from_float(x) ) {}
fixed::fixed(double x) : value_(  fix16_from_dbl(x)   ) {}

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
fixed fixed::operator+ (fixed const & rhs) const { return from_value(fix16_add(value_, rhs.value_)); }
fixed fixed::operator- (fixed const & rhs) const { return from_value(fix16_sub(value_, rhs.value_)); }
fixed fixed::operator* (fixed const & rhs) const { return from_value(fix16_mul(value_, rhs.value_)); }
fixed fixed::operator/ (fixed const & rhs) const { return from_value(fix16_div(value_, rhs.value_)); }

fixed  fixed::operator++(int)               { fixed result = *this; ++(*this); return result; }
fixed  fixed::operator--(int)               { fixed result = *this; --(*this); return result; }
fixed& fixed::operator++()                  { value_ = fix16_add(value_, One.value_); return *this; }
fixed& fixed::operator--()                  { value_ = fix16_sub(value_, One.value_); return *this; }
fixed& fixed::operator+=(fixed const & rhs) { value_ = fix16_add(value_, rhs.value_); return *this; }
fixed& fixed::operator-=(fixed const & rhs) { value_ = fix16_sub(value_, rhs.value_); return *this; }
fixed& fixed::operator*=(fixed const & rhs) { value_ = fix16_mul(value_, rhs.value_); return *this; }
fixed& fixed::operator/=(fixed const & rhs) { value_ = fix16_div(value_, rhs.value_); return *this; }

}} // math::fpm
