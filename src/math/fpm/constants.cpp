#include <fix16.h>
#include "constants.h"
#include "fixed.h"
#include "functions.h"

namespace math
{
	namespace fpm
	{
		const fixed Max      = from_value(fix16_maximum);
		const fixed Min      = from_value(fix16_minimum);
		const fixed Overflow = from_value(fix16_overflow);
		const fixed One      = from_value(fix16_one);
		const fixed Half     = from_value(fix16_one / 2);
		const fixed Pi       = from_value(fix16_pi);
		const fixed E        = from_value(fix16_e);
		const fixed Zero     = from_value(0);
	}
}
