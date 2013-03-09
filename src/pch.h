#if !defined(WIN32) && !defined(UNIX) && !defined(OSX) && !defined(IPHONE) && !defined(IPAD)
	#error "Missing preprocessor definition. Define one of these: WIN32, UNIX, OSX, IPHONE, IPAD"
#endif

#if defined(IPHONE) || defined(IPAD)
	#define IOS
#endif

#if defined(OSX) || defined(IOS)
	#define APPLE
#endif

#include <stdint.h>
#include <iostream>

#define error_log(s) (std::cerr << s << std::endl)
