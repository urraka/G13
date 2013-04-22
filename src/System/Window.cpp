#include <System/System.h>

#if defined(IOS)
	#include <System/ios.h>
#else
	#include <GL/glfw.h>
#endif

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
			ResizeEvent event;
			event.rotation = window->rotation();
			window->size(event.width, event.height);
			window->push(Event(event));
		}
	#else
		int GLFWCALL close()
		{
			window->push(Event(Event::Close));
			return GL_TRUE;
		}

		void GLFWCALL resize(int width, int height)
		{
			ResizeEvent event;
			event.rotation = window->rotation();
			window->size(event.width, event.height);
			window->push(Event(event));
		}

		void GLFWCALL keyboard(int key, int action)
		{
			KeyboardEvent event;
			event.key = static_cast<Keyboard::Key>(key);
			event.pressed = (action == GLFW_PRESS);
			window->push(Event(event));
		}

		void GLFWCALL character(int ch, int action)
		{
			if (action == GLFW_PRESS)
			{
				CharEvent event;
				event.ch = ch;
				window->push(Event(event));
			}
		}

		void GLFWCALL mouse(int button, int action)
		{
			MouseEvent event;
			event.button = static_cast<Mouse::Button>(button);
			event.pressed = (action == GLFW_PRESS);
			window->push(Event(event));
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

void Window::create(bool fullscreen, int fsaa)
{
	#if defined(IOS)
		iosCreateWindow();
		iosSetOrientationCallback(callbacks::ios_orientation);
	#else
		if (glfwInit() == GL_FALSE)
		{
			std::cerr << "Error initializing GLFW." << std::endl;
			return;
		}

		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2);
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 0);
		glfwOpenWindowHint(GLFW_FSAA_SAMPLES, fsaa);

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
		#define get_size iosGetWindowSize
	#else
		#define get_size glfwGetWindowSize
	#endif

	if (abs(rotation()) == 90)
		get_size(&height, &width);
	else
		get_size(&width, &height);

	#undef get_size
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
