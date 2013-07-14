#include "sys.h"
#include <vector>
#include <string>
#include <iostream>
#include <assert.h>
#include <cmath>

#ifdef IOS
	#include "ios.h"
	#include <mach/mach_time.h>
#else
	#include <GL/glew.h>
	#include <GLFW/glfw3.h>
#endif

namespace sys {

// -----------------------------------------------------------------------------
// Internal
// -----------------------------------------------------------------------------

struct System
{
	enum MetricsMode { Absolute, Relative };

	struct Callbacks
	{
		Callbacks() : initialize(0), display(0), terminate(0) {}
		Callback initialize;
		Callback display;
		Callback terminate;
	};

	System()
		:	initialized(false),
			pollIndex(0),
			window(0),
			title("OpenGL Application"),
			sizeMode(Relative),
			posMode(Relative),
			width(0),
			height(0),
			r_width(0.5f),
			r_height(0.5f),
			x(0),
			y(0),
			r_x(0.25f),
			r_y(0.25f),
			samples(0),
			fullscreen(false),
			vsync(0)
	{}

	bool                initialized;
	int                 pollIndex;
	std::vector<Event*> events;
	Callbacks           callbacks;
	GLFWwindow         *window;
	std::string         title;
	MetricsMode         sizeMode;
	MetricsMode         posMode;
	int                 width;
	int                 height;
	float               r_width;
	float               r_height;
	int                 x;
	int                 y;
	float               r_x;
	float               r_y;
	int                 samples;
	bool                fullscreen;
	int                 vsync;

	#ifdef IOS
		double timerBase;
		double timerResolution;
	#endif
};

static System sys;

template<typename T> static std::vector<T> &event_storage()
{
	static std::vector<T> e;
	return e;
}

template<typename T> static void push_event(const T &event)
{
	event_storage<T>().push_back(event);
	sys.events.push_back(&event_storage<T>().back());
}

static void clear_events();
static void cb_size(GLFWwindow *window, int width, int height);
static void cb_keyboard(GLFWwindow *window, int key, int code, int action, int mods);
static void cb_mouse(GLFWwindow *window, int button, int action, int mods);
static void cb_char(GLFWwindow *window, unsigned int ch);

#ifdef IOS
	static void cb_orientation();
#endif

// -----------------------------------------------------------------------------
// Application callbacks
// -----------------------------------------------------------------------------

void callback(CallbackType type, Callback callback)
{
	assert(!sys.initialized);

	switch (type)
	{
		case Initialize: sys.callbacks.initialize = callback; break;
		case Display   : sys.callbacks.display    = callback; break;
		case Terminate : sys.callbacks.terminate  = callback; break;
	}
}

// -----------------------------------------------------------------------------
// Run
// -----------------------------------------------------------------------------

int run(int argc, char *argv[])
{
	assert(sys.callbacks.initialize != 0);
	assert(sys.callbacks.display    != 0);
	assert(sys.callbacks.terminate  != 0);

	#ifdef IOS
		iosSetAppLaunchedCallback(sys.callbacks.initialize);
		iosSetDisplayCallback(sys.callbacks.display);
		iosSetTerminateCallback(sys.callbacks.terminate);
		return iosRun(argc, argv);
	#else
		sys.callbacks.initialize();

		if (sys.window != 0)
		{
			while (!exiting())
			{
				sys.callbacks.display();
				glfwSwapBuffers(sys.window);
				glfwPollEvents();
			}
		}

		sys.callbacks.terminate();
		glfwTerminate();
		return 0;
	#endif
}

// -----------------------------------------------------------------------------
// Initialize
// -----------------------------------------------------------------------------

void initialize()
{
	assert(!sys.initialized);

	#ifdef IOS
		iosCreateWindow();
		iosSetOrientationCallback(cb_orientation);

		// timer
		mach_timebase_info_data_t info;
		mach_timebase_info(&info);
		sys.timerBase = mach_absolute_time();
		sys.timerResolution = (double)info.numer / (info.denom * 1.0e9);
	#else
		if (glfwInit() == GL_FALSE)
		{
			std::cerr << "Error initializing GLFW." << std::endl;
			return;
		}

		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *mode = glfwGetVideoMode(monitor);

		int x = sys.x;
		int y = sys.y;
		int w = sys.width;
		int h = sys.height;
		const char *title = sys.title.c_str();

		if (sys.sizeMode == System::Relative)
		{
			w = mode->width * sys.r_width;
			h = mode->height * sys.r_height;
		}

		if (sys.posMode == System::Relative)
		{
			x = mode->width * sys.r_x;
			y = mode->height * sys.r_y;
		}

		if (!sys.fullscreen)
			monitor = 0;

		glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
		glfwWindowHint(GLFW_SAMPLES, sys.samples);

		sys.window = glfwCreateWindow(w, h, title, monitor, 0);

		if (sys.window == 0)
		{
			std::cerr << "Error creating window." << std::endl;
			return;
		}

		glfwSetWindowPos(sys.window, x, y);
		glfwShowWindow(sys.window);
		glfwMakeContextCurrent(sys.window);
		glfwSwapInterval(sys.vsync);

		#ifdef DEBUG
			int mayor   = glfwGetWindowAttrib(sys.window, GLFW_CONTEXT_VERSION_MAJOR);
			int minor   = glfwGetWindowAttrib(sys.window, GLFW_CONTEXT_VERSION_MINOR);
			int rev     = glfwGetWindowAttrib(sys.window, GLFW_CONTEXT_REVISION);
			int profile = glfwGetWindowAttrib(sys.window, GLFW_OPENGL_PROFILE);

			std::cout << "OpenGL context initialized." << std::endl;
			std::cout << "Version: " << mayor << "." << minor << "." << rev << std::endl;
			std::cout << "Profile: ";

			switch (profile)
			{
				case GLFW_OPENGL_CORE_PROFILE:   std::cout << "core.";;
				case GLFW_OPENGL_COMPAT_PROFILE: std::cout << "compatible.";;
				case GLFW_OPENGL_ANY_PROFILE:    std::cout << "unknown.";;
			}

			std::cout << std::endl;
		#endif

		if (glewInit() != GLEW_OK)
			std::cerr << "Error initializing GLEW." << std::endl;

		glfwSetWindowSizeCallback (sys.window, cb_size);
		glfwSetCharCallback       (sys.window, cb_char);
		glfwSetMouseButtonCallback(sys.window, cb_mouse);
		glfwSetKeyCallback        (sys.window, cb_keyboard);
	#endif

	sys.initialized = true;
}

void exit()
{
	assert(sys.initialized);

	#ifndef IOS
		glfwSetWindowShouldClose(sys.window, GL_TRUE);
	#endif
}

// -----------------------------------------------------------------------------
// State
// -----------------------------------------------------------------------------

bool exiting()
{
	assert(sys.initialized);
	return glfwWindowShouldClose(sys.window);
}

void fullscreen(bool enable)
{
	assert(!sys.initialized);
	sys.fullscreen = enable;
}

void vsync(int interval)
{
	#ifndef IOS
		if (sys.initialized)
			glfwSwapInterval(interval);
		else
			sys.vsync = interval;
	#endif
}

void samples(int samples)
{
	assert(!sys.initialized);
	sys.samples = samples;
}

void window_title(const char *title)
{
	#ifndef IOS
		if (sys.initialized)
			glfwSetWindowTitle(sys.window, title);
		else
			sys.title = title;
	#endif
}

void window_size(int width, int height)
{
	#ifndef IOS
		if (sys.initialized)
		{
			glfwSetWindowSize(sys.window, width, height);
		}
		else
		{
			sys.sizeMode = System::Absolute;
			sys.width = width;
			sys.height = height;
		}
	#endif
}

void window_size(float width, float height)
{
	#ifndef IOS
		if (sys.initialized)
		{
			GLFWmonitor *monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode *mode = glfwGetVideoMode(monitor);
			glfwSetWindowSize(sys.window, mode->width * width, mode->height * height);
		}
		else
		{
			sys.sizeMode = System::Relative;
			sys.r_width = width;
			sys.r_height = height;
		}
	#endif
}

void window_position(int x, int y)
{
	#ifndef IOS
		if (sys.initialized)
		{
			glfwSetWindowPos(sys.window, x, y);
		}
		else
		{
			sys.posMode = System::Absolute;
			sys.x = x;
			sys.y = y;
		}
	#endif
}

void window_position(float x, float y)
{
	#ifndef IOS
		if (sys.initialized)
		{
			GLFWmonitor *monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode *mode = glfwGetVideoMode(monitor);
			glfwSetWindowPos(sys.window, mode->width * x, mode->height * y);
		}
		else
		{
			sys.posMode = System::Relative;
			sys.r_x = x;
			sys.r_y = y;
		}
	#endif
}

int window_rotation()
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

void window_size(int *width, int *height)
{
	assert(sys.initialized);

	#if defined(IOS)
		#define get_size(wnd, w, h) iosGetWindowSize(w, h)
	#else
		#define get_size(wnd, w, h) glfwGetWindowSize(wnd, w, h)
	#endif

	if (std::abs(window_rotation()) == 90)
		get_size(sys.window, height, width);
	else
		get_size(sys.window, width, height);

	#undef get_size
}

// -----------------------------------------------------------------------------
// Input
// -----------------------------------------------------------------------------

bool pressed(int code)
{
	assert(sys.initialized);

	#ifndef IOS
		if (code >= MouseButton1)
			return glfwGetMouseButton(sys.window, code - MouseButton1) == GLFW_PRESS;
		else
			return glfwGetKey(sys.window, code) == GLFW_PRESS;
	#endif
}

void mouse(double *x, double *y)
{
	assert(sys.initialized);

	#ifndef IOS
		glfwGetCursorPos(sys.window, x, y);
	#endif
}

// -----------------------------------------------------------------------------
// Time
// -----------------------------------------------------------------------------

Time time()
{
	assert(sys.initialized);

	#ifdef IOS
		double t = (double)(mach_absolute_time() - sys.timerBase) * sys.timerResolution;
	#else
		double t = glfwGetTime();
	#endif

	return (Time)(t * (double)Seconds);
}

double to_seconds(Time value)
{
	return (double)value / (double)Seconds;
}

double to_milliseconds(Time value)
{
	return (double)value / (double)Milliseconds;
}

// -----------------------------------------------------------------------------
// Events
// -----------------------------------------------------------------------------

Event *poll_events()
{
	if (sys.pollIndex == (int)sys.events.size())
	{
		clear_events();
		return 0;
	}

	return sys.events[sys.pollIndex++];
}

void clear_events()
{
	event_storage<ResizeEvent  >().clear();
	event_storage<KeyboardEvent>().clear();
	event_storage<MouseEvent   >().clear();
	event_storage<CharEvent    >().clear();

	sys.events.clear();
	sys.pollIndex = 0;
}

#ifdef IOS
	void cb_orientation()
	{
		cb_size(sys.window, 0, 0);
	}
#endif

void cb_size(GLFWwindow *window, int width, int height)
{
	ResizeEvent event;
	event.rotation = window_rotation();
	window_size(&event.width, &event.height);
	push_event(event);
}

void cb_keyboard(GLFWwindow *window, int key, int code, int action, int mods)
{
	KeyboardEvent event;
	event.key = key;
	event.pressed = action == GLFW_PRESS;
	push_event(event);
}

void cb_mouse(GLFWwindow *window, int button, int action, int mods)
{
	MouseEvent event;
	event.button = button + MouseButton1;
	event.pressed = action == GLFW_PRESS;
	push_event(event);
}

void cb_char(GLFWwindow *window, unsigned int ch)
{
	CharEvent event;
	event.ch = ch;
	push_event(event);
}

} // sys
