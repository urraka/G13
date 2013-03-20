#include <pch.h>

#if !defined(IOS)

#include <GL/glfw.h>
#include <System/Window.h>

Window::~Window()
{
	glfwTerminate();
}

bool Window::init(bool fullscreen)
{
	if (glfwInit() == GL_FALSE)
	{
		error_log("Error initializing GLFW.");
		return false;
	}

	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1);

	GLFWvidmode mode;
	glfwGetDesktopMode(&mode);

	int result = 0;

	if (fullscreen)
	{
		result = glfwOpenWindow(mode.Width, mode.Height, 0, 0, 0, 0, 0, 0, GLFW_FULLSCREEN);
	}
	else
	{
		result = glfwOpenWindow(mode.Width * 0.8f, mode.Height * 0.8f, 0, 0, 0, 0, 0, 0, GLFW_WINDOW);
		glfwSetWindowPos(mode.Width * 0.1f, mode.Height * 0.1f);
	}

	if (result == GL_FALSE)
	{
		error_log("Error creating application window or OpenGL context.");
		return false;
	}

	int mayor, minor, rev;

	glfwGetGLVersion(&mayor, &minor, &rev);
	std::cout << "OpenGL context initialized. Version: " << mayor << "." << minor << "." << rev << std::endl;

	return true;
}

void Window::title(const char *title)
{
	glfwSetWindowTitle(title);
}

void Window::vsync(bool enable)
{
	glfwSwapInterval(enable ? 1 : 0);
}

ivec2 Window::size()
{
	int w, h;
	glfwGetWindowSize(&w, &h);
	return ivec2(w, h);
}

void Window::display()
{
	glfwSwapBuffers();
}

#endif
