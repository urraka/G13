#pragma once

class Window;

class Application
{
public:
	Application(int argc, char *argv[]);
	~Application();

	int run();

	typedef void (*LaunchCallback)(Application*);
	typedef void (*TerminateCallback)();

	void launchCallback(LaunchCallback callback);
	void terminateCallback(TerminateCallback callback);

	Window *window(bool fullscreen, int fsaa = 0);

private:
	int argc_;
	char **argv_;
	Window *window_;

	LaunchCallback launchCallback_;
	TerminateCallback terminateCallback_;

	#if defined(IOS)
		static void iosLaunch();
		static void iosTerminate();
	#endif
};
