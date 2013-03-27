#include <System/platform.h>
#if defined(IOS)
	#include <System/ios.h>
#else
	#include <GL/glfw.h>
#endif
#include <System/Application.h>
#include <System/Window.h>

#include <iostream>

namespace { namespace callbacks {

	Window *window = 0;
	Window::DisplayCallback displayCallback = 0;

	#if defined(IOS)
		void ios_display()
		{
			displayCallback();
		}

		void ios_orientation()
		{
			int r = window->rotation();

			if (r == 90 || r == -90)
				window->push(Event(Event::Resize, height, width, r));
			else
				window->push(Event(Event::Resize, width, height, r));
		}
	#else
		int GLFWCALL close()
		{
			window->push(Event(Event::Close));
			return GL_TRUE;
		}

		void GLFWCALL resize(int width, int height)
		{
			int r = window->rotation();

			if (r == 90 || r == -90)
				window->push(Event(Event::Resize, height, width, r));
			else
				window->push(Event(Event::Resize, width, height, r));
		}

		void GLFWCALL keyboard(int key, int action)
		{
			window->push(Event(action == GLFW_PRESS ? Event::KeyPress : Event::KeyRelease, key));
		}

		void GLFWCALL character(int ch, int action)
		{
			window->push(Event(action == GLFW_PRESS ? Event::CharPress : Event::CharRelease, ch));
		}

		void GLFWCALL mouse(int button, int action)
		{
			window->push(Event(action == GLFW_PRESS ? Event::MouseButtonPress : Event::MouseButtonRelease, button));
		}
	#endif

}}

Window::Window()
	:	pollIndex_(0)
{
	callbacks::window = this;
	events_.reserve(20);
}

Window::~Window()
{
	#if !defined(IOS)
		glfwSetWindowCloseCallback(0);
		glfwSetWindowSizeCallback(0);
		glfwSetKeyCallback(0);
		glfwSetCharCallback(0);
		glfwSetMouseButtonCallback(0);
	#endif
}

void Window::create(bool fullscreen)
{
	#if defined(IOS)
		iosCreateWindow();
		iosSetOrientationCallback(ios_orientation);
	#else
		if (glfwInit() == GL_FALSE)
		{
			std::cerr << "Error initializing GLFW." << std::endl;
			return;
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
			std::cerr << "Error creating application window or OpenGL context." << std::endl;
			return;
		}

		int mayor, minor, rev;

		glfwGetGLVersion(&mayor, &minor, &rev);
		std::cout << "OpenGL context initialized. Version: " << mayor << "." << minor << "." << rev << std::endl;

		glfwSetWindowCloseCallback(callbacks::close);
		glfwSetWindowSizeCallback(callbacks::resize);
		glfwSetKeyCallback(callbacks::keyboard);
		glfwSetCharCallback(callbacks::character);
		glfwSetMouseButtonCallback(callbacks::mouse);
	#endif
}

void Window::title(const char *title)
{
	#if !defined(IOS)
		glfwSetWindowTitle(title);
	#endif
}

void Window::vsync(bool enable)
{
	#if !defined(IOS)
		glfwSwapInterval(enable ? 1 : 0);
	#endif
}

void Window::size(int &width, int &height)
{
	#if defined(IOS)
		iosGetWindowSize(&width, &height);
	#else
		glfwGetWindowSize(&width, &height);
	#endif

	int r = rotation();

	if (r == 90 || r == -90)
	{
		int tmp = width;
		width = height;
		height = tmp;
	}
}

void Window::display(DisplayCallback callback)
{
	callbacks::displayCallback = callback;

	#if defined(IOS)
		iosSetDisplayCallback(callbacks::ios_display);
	#endif
}

void Window::events()
{
	callbacks::displayCallback();

	#if !defined(IOS)
		glfwSwapBuffers();
	#endif
}

void Window::push(const Event &event)
{
	events_.push_back(event);
}

bool Window::poll(Event *event)
{
	if (pollIndex_ == events_.size())
	{
		events_.clear();
		pollIndex_ = 0;
		return false;
	}

	*event = events_[pollIndex_++];
	return true;
}

void Window::close()
{
	#if !defined(IOS)
		glfwCloseWindow();
	#endif
}

int Window::rotation()
{
	#if defined(IOS)
		int orient = iosGetCurrentOrientation();

		switch (orient)
		{
			case IOS_ORIENTATION_PORTRAIT:
				return 0;

			case IOS_ORIENTATION_PORTRAIT_UPSIDE_DOWN:
				return 180;

			case IOS_ORIENTATION_LANDSCAPE_LEFT:
				return -90;

			case IOS_ORIENTATION_LANDSCAPE_RIGHT:
				return 90;

			default:
				return 0;
		}
	#else
		return 0;
	#endif
}
