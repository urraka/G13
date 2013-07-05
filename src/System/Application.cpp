#include "platform.h"
#include "Window.h"
#include "Application.h"

#if defined(IOS)
	#include "ios.h"
#else
	#include <GL/glfw.h>
#endif

#include <assert.h>

namespace
{
	Application *self = 0;
}

Application::Application(int argc, char *argv[])
	:	argc_(argc),
		argv_(argv),
		window_(0),
		launchCallback_(0),
		terminateCallback_(0)
{
	self = this;
}

Application::~Application()
{
	if (window_)
		delete window_;
}

void Application::launchCallback(LaunchCallback callback)
{
	launchCallback_ = callback;
}

void Application::terminateCallback(TerminateCallback callback)
{
	terminateCallback_ = callback;
}

Window *Application::window(bool fullscreen, int fsaa)
{
	assert(window_ == 0);

	window_ = new Window();
	window_->create(fullscreen, fsaa);

	return window_;
}

int Application::run()
{
	assert(launchCallback_ != 0 && terminateCallback_ != 0);

	#if defined(IOS)
		iosSetAppLaunchedCallback(Application::iosLaunch);
		iosSetTerminateCallback(Application::iosTerminate);

		return iosRun(argc_, argv_);
	#else
		launchCallback_(this);

		assert(window_ != 0);

		while (glfwGetWindowParam(GLFW_OPENED) == GL_TRUE)
			window_->events();

		terminateCallback_();

		delete window_;
		window_ = 0;

		glfwTerminate();

		return 0;
	#endif
}

#if defined(IOS)
	void Application::iosLaunch()
	{
		self->launchCallback_(self);
	}

	void Application::iosTerminate()
	{
		self->terminateCallback_();
	}
#endif
