#include "platform.h"
#include "Keyboard.h"

#if !defined(IOS)
	#include <GL/glfw.h>
#endif

bool Keyboard::pressed(Key key)
{
	#if !defined(IOS)
		return (glfwGetKey(key) == GLFW_PRESS);
	#else
		return false;
	#endif
}
