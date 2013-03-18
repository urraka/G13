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
#include <assert.h>

#include <iostream>
#include <string>
#include <vector>
#include <stack>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#define error_log(s) (std::cerr << s << std::endl)

using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::ivec2;
