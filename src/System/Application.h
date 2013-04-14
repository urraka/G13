#pragma once

class Window;

class Application
{
public:
	Application(int argc, char *argv[]);
	~Application();

	int run();

	typedef void (*LaunchedCallback)(Application*);
	typedef void (*TerminateCallback)();

	void launched(LaunchedCallback callback);
	void terminate(TerminateCallback callback);

	Window *window(bool fullscreen, int fsaa = 0);

private:
	int argc_;
	char **argv_;
	Window *window_;
};
