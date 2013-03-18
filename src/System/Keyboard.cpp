#include <pch.h>
#if !defined(IOS)
	#include <GL/glfw.h>
#endif
#include <System/Keyboard.h>

namespace Keyboard
{
	bool pressed(Key key)
	{
		#if defined(IOS)
			return (glfwGetKey(key) == GLFW_PRESS);
		#else
			return false;
		#endif
	}
}
