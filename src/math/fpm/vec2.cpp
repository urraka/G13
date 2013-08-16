#include "vec2.h"
#include <ostream>

std::ostream& operator<<(std::ostream & stream, math::fpm::vec2 const & x)
{
	return stream << "(" << x.x << "," << x.y << ")";
}
