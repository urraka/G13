#include <System/System.h>
#if !defined(IOS)
	#include <GL/glfw.h>
#endif

void Mouse::position(int &x, int &y)
{
	#if !defined(IOS)
		glfwGetMousePos(&x, &y);
	#else
		x = y = 0;
	#endif
}
