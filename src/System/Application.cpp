#include <System/platform.h>
#if defined(IOS)
	#include <System/ios.h>
#else
	#include <GL/glfw.h>
#endif
#include <System/Application.h>
#include <System/Window.h>

#include <assert.h>

namespace
{
	Application *app = 0;

	namespace callbacks
	{
		Application::LaunchedCallback launchedCallback = 0;
		Application::TerminateCallback terminateCallback = 0;

		#if defined(IOS)
			void ios_launched() { launchedCallback(app); }
			void ios_terminate() { terminateCallback(); }
		#endif
	}
}

Application::Application(int argc, char *argv[])
	:	argc_(argc),
		argv_(argv),
		window_(0)
{
	app = this;
}

void Application::launched(LaunchedCallback callback)
{
	callbacks::launchedCallback = callback;

	#if defined(IOS)
		iosSetAppLaunchedCallback(callbacks::ios_launched);
	#endif
}

void Application::terminate(TerminateCallback callback)
{
	callbacks::terminateCallback = callback;

	#if defined(IOS)
		iosSetTerminateCallback(callbacks::ios_terminate);
	#endif
}

Window *Application::window(bool fullscreen)
{
	assert(window_ == 0);

	window_ = new Window();
	window_->create(fullscreen);

	return window_;
}

int Application::run()
{
	#if defined(IOS)
		return iosRun(argc_, argv_);
	#else
		callbacks::launchedCallback(app);
		assert(window_ != 0);

		while (glfwGetWindowParam(GLFW_OPENED) == GL_TRUE)
			window_->events();

		if (callbacks::terminateCallback != 0)
			callbacks::terminateCallback();

		delete window_;
		glfwTerminate();

		return 0;
	#endif
}