#include <fstream>
#include <sstream>
#include <string>

namespace hlp
{
	inline std::string read(const char *filename)
	{
		std::ifstream file(filename);
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}
}
