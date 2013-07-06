#pragma once

#include <string>
#include <sstream>
#include <vector>

namespace hlp {

typedef std::vector<std::string> strvector;

inline strvector split(const char *str, char delimiter)
{
	strvector result;
	std::stringstream ss(str);
	std::string item;

	while (std::getline(ss, item, delimiter))
		result.push_back(item);

	return result;
}

inline strvector split(const std::string &str, char delimiter)
{
	return split(str.c_str(), delimiter);
}

} // hlp
