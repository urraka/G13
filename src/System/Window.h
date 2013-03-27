#pragma once

#include <cstddef>
#include <vector>

#include <System/Event.h>
#include <System/Keyboard.h>

class Application;

class Window
{
public:
	Window();
	~Window();

	typedef void (*DisplayCallback)();

	void create(bool fullscreen = true);
	void title(const char *title);
	void vsync(bool enable);
	void size(int &width, int &height);
	void display(DisplayCallback callback);
	void push(const Event &event);
	bool poll(Event *event);
	void close();
	int rotation();

private:
	void events();

	std::vector<Event> events_;
	size_t pollIndex_;

	friend class Application;
};
