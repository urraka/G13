#include "sys.h"
#include <vector>
#include <string>
#include <iostream>
#include <assert.h>
#include <cmath>

#ifdef IOS
	#include "ios.h"
	#include <mach/mach_time.h>

	typedef void GLFWwindow;
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
			cursorMode(Normal),
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
	std::vector<Event>  events;
	Callbacks           callbacks;
	GLFWwindow         *window;

	// initial state
	std::string         title;
	CursorMode          cursorMode;
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

static void clear_events();
static void push_event(const Event &event);

#ifdef IOS
	static void cb_orientation();
#else
	static void cb_size(GLFWwindow *window, int width, int height);
	static void cb_framebuffer(GLFWwindow *window, int width, int height);
	static void cb_keyboard(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void cb_mouse(GLFWwindow *window, int button, int action, int mods);
	static void cb_char(GLFWwindow *window, unsigned int ch);
	static void cb_mousemove(GLFWwindow *window, double x, double y);
	static void cb_mouseenter(GLFWwindow *window, int entered);
	static void cb_scroll(GLFWwindow *window, double xoffset, double yoffset);
	static void cb_focus(GLFWwindow *window, int focused);
	static void cb_close(GLFWwindow *window);
	static void cb_refresh(GLFWwindow *window);
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
			while (!glfwWindowShouldClose(sys.window))
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

		switch (sys.cursorMode)
		{
			case Normal:
				glfwSetInputMode(sys.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				break;

			case Hidden:
				glfwSetInputMode(sys.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				break;

			case Disabled:
				glfwSetInputMode(sys.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				break;
		}

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

		glfwSetWindowSizeCallback     (sys.window, cb_size);
		glfwSetFramebufferSizeCallback(sys.window, cb_framebuffer);
		glfwSetKeyCallback            (sys.window, cb_keyboard);
		glfwSetMouseButtonCallback    (sys.window, cb_mouse);
		glfwSetCharCallback           (sys.window, cb_char);
		glfwSetCursorPosCallback      (sys.window, cb_mousemove);
		glfwSetCursorEnterCallback    (sys.window, cb_mouseenter);
		glfwSetScrollCallback         (sys.window, cb_scroll);
		glfwSetWindowFocusCallback    (sys.window, cb_focus);
		glfwSetWindowCloseCallback    (sys.window, cb_close);
		glfwSetWindowRefreshCallback  (sys.window, cb_refresh);
	#endif

	sys.initialized = true;
}

// -----------------------------------------------------------------------------
// Exit
// -----------------------------------------------------------------------------

void exit()
{
	assert(sys.initialized);

	#ifndef IOS
		glfwSetWindowShouldClose(sys.window, GL_TRUE);
	#endif
}

// -----------------------------------------------------------------------------
// Screen/window
// -----------------------------------------------------------------------------

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
		if (std::abs(window_rotation()) == 90)
			iosGetWindowSize(height, width);
		else
			iosGetWindowSize(width, height);
	#else
		glfwGetWindowSize(sys.window, width, height);
	#endif
}

int window_width()
{
	int w, h;
	window_size(&w, &h);
	return w;
}

int window_height()
{
	int w, h;
	window_size(&w, &h);
	return h;
}

void framebuffer_size(int *width, int *height)
{
	assert(sys.initialized);

	#if defined(IOS)
		window_size(width, height);
	#else
		glfwGetFramebufferSize(sys.window, width, height);
	#endif
}

int framebuffer_width()
{
	int w, h;
	framebuffer_size(&w, &h);
	return w;
}

int framebuffer_height()
{
	int w, h;
	framebuffer_size(&w, &h);
	return h;
}

// -----------------------------------------------------------------------------
// Clipboard
// -----------------------------------------------------------------------------

const char *clipboard()
{
	assert(sys.initialized);

	const char *result = 0;

	#ifndef IOS
		result = glfwGetClipboardString(sys.window);
	#endif

	return result;
}

void clipboard(const char *str)
{
	assert(sys.initialized);

	#ifndef IOS
		glfwSetClipboardString(sys.window, str);
	#endif
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
	#else
		return false;
	#endif
}

void mouse(double *x, double *y)
{
	assert(sys.initialized);

	#ifndef IOS
		glfwGetCursorPos(sys.window, x, y);
	#else
		*x = *y = 0;
	#endif
}

double mousex()
{
	double x, y;
	mouse(&x, &y);
	return x;
}

double mousey()
{
	double x, y;
	mouse(&x, &y);
	return y;
}

// -----------------------------------------------------------------------------
// Cursor
// -----------------------------------------------------------------------------

Cursor create_cursor(int width, int height, int cx, int cy, const void *data)
{
	Cursor cursor = 0;

	#ifndef IOS
		if (sys.initialized)
			cursor = glfwCreateCursor(width, height, cx, cy, 0, data);
	#endif

	return cursor;
}

void set_cursor(Cursor cursor)
{
	#ifndef IOS
		if (sys.initialized)
			glfwSetCursor(sys.window, (GLFWcursor*)cursor);
	#endif
}

void cursor_mode(CursorMode mode)
{
	#ifndef IOS
		if (sys.initialized)
		{
			switch (mode)
			{
				case Normal:
					glfwSetInputMode(sys.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					break;

				case Hidden:
					glfwSetInputMode(sys.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
					break;

				case Disabled:
					glfwSetInputMode(sys.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					break;
			}
		}
		else
		{
			sys.cursorMode = mode;
		}
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

	return &sys.events[sys.pollIndex++];
}

void clear_events()
{
	sys.events.clear();
	sys.pollIndex = 0;
}

void push_event(const Event &event)
{
	sys.events.push_back(event);
}

// -----------------------------------------------------------------------------
// Event callbacks
// -----------------------------------------------------------------------------

#ifdef IOS

void cb_orientation()
{
	Event event;

	event.type = Resize;
	event.size.rotation = window_rotation();

	window_size(&event.size.width, &event.size.height);

	event.size.fboWidth = event.size.width;
	event.size.fboHeight = event.size.height;

	push_event(event);
}

#else

void cb_size(GLFWwindow *window, int width, int height)
{
	Event event;

	event.type = Resize;
	event.size.rotation = 0;
	event.size.width = width;
	event.size.height = height;

	glfwGetFramebufferSize(sys.window, &event.size.fboWidth, &event.size.fboHeight);

	push_event(event);
}

void cb_framebuffer(GLFWwindow *window, int width, int height)
{
	Event event;

	event.type = Resize;
	event.size.rotation = 0;
	event.size.fboWidth = width;
	event.size.fboHeight = height;

	glfwGetWindowSize(sys.window, &event.size.width, &event.size.height);

	push_event(event);
}

void cb_keyboard(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	Event event;

	switch (action)
	{
		case GLFW_PRESS  : event.type = KeyPress;   break;
		case GLFW_RELEASE: event.type = KeyRelease; break;
		case GLFW_REPEAT : event.type = KeyRepeat;  break;
		default: return;
	}

	event.key.code     = key;
	event.key.scancode = scancode;
	event.key.shift    = mods & GLFW_MOD_SHIFT;
	event.key.ctrl     = mods & GLFW_MOD_CONTROL;
	event.key.alt      = mods & GLFW_MOD_ALT;
	event.key.super    = mods & GLFW_MOD_SUPER;

	push_event(event);
}

void cb_mouse(GLFWwindow *window, int button, int action, int mods)
{
	Event event;

	switch (action)
	{
		case GLFW_PRESS  : event.type = MouseButtonPress;   break;
		case GLFW_RELEASE: event.type = MouseButtonRelease; break;
		default: return;
	}

	event.mouseButton.code  = button + MouseButton1;
	event.mouseButton.shift = mods & GLFW_MOD_SHIFT;
	event.mouseButton.ctrl  = mods & GLFW_MOD_CONTROL;
	event.mouseButton.alt   = mods & GLFW_MOD_ALT;
	event.mouseButton.super = mods & GLFW_MOD_SUPER;

	push_event(event);
}

void cb_char(GLFWwindow *window, unsigned int ch)
{
	Event event;

	event.type = Text;
	event.text.ch = ch;

	push_event(event);
}

void cb_mousemove(GLFWwindow *window, double x, double y)
{
	Event event;

	event.type = MouseMove;
	event.mouseMove.x = x;
	event.mouseMove.y = y;

	push_event(event);
}

void cb_mouseenter(GLFWwindow *window, int entered)
{
	Event event;

	event.type = (entered == GL_TRUE ? MouseEnter : MouseLeave);

	push_event(event);
}

void cb_scroll(GLFWwindow *window, double xoffset, double yoffset)
{
	Event event;

	event.type = MouseWheelMove;
	event.mouseWheel.xoffset = xoffset;
	event.mouseWheel.yoffset = yoffset;

	push_event(event);
}

void cb_focus(GLFWwindow *window, int focused)
{
	Event event;

	event.type = FocusChange;
	event.focus.focused = (focused == GL_TRUE);

	push_event(event);
}

void cb_close(GLFWwindow *window)
{
	Event event;
	event.type = Close;
	push_event(event);
}

void cb_refresh(GLFWwindow *window)
{
	sys.callbacks.display();
	glfwSwapBuffers(window);
}

#endif

} // sys
