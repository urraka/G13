#include <string>
#include <stdlib.h>

namespace hlp
{
	inline int to_int(const char *str)
	{
		return atoi(str);
	}

	inline int to_int(const std::string &str)
	{
		return atoi(str.c_str());
	}
}
