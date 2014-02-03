#pragma once

#include <iostream>

#ifdef DEBUG
	#define debug_log(x) std::cout << "[Debug] " << x << std::endl
#else
	#define debug_log(x)
#endif

#define error_log(x) std::cerr << "[Error] " << x << std::endl
