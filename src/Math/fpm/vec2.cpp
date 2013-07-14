#include "vec2.h"

std::ostream& operator<<(std::ostream & stream, math::fpm::vec2 const & x)
{
	return stream << "(" << x.x << "," << x.y << ")";
}
