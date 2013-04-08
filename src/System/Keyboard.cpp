#include <System/platform.h>
#if !defined(IOS)
	#include <GL/glfw.h>
#endif
#include <System/Keyboard.h>

bool Keyboard::pressed(Key key)
{
	#if !defined(IOS)
		return (glfwGetKey(key) == GLFW_PRESS);
	#else
		return false;
	#endif
}
