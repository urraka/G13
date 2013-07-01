#include <string>
#include <cstring>

namespace hlp
{
	template<size_t N> void assign(char (&dest)[N], const char *src)
	{
		strncpy(dest, src, N);
		dest[N - 1] = 0;
	}
}
