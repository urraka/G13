#pragma once

class Window;

class Application
{
public:
	Application(int argc, char *argv[]);

	int run();

	typedef void (*LaunchedCallback)(Application*);
	typedef void (*TerminateCallback)();

	void launched(LaunchedCallback callback);
	void terminate(TerminateCallback callback);

	Window *window(bool fullscreen);

private:
	int argc_;
	char **argv_;
	Window *window_;
};
