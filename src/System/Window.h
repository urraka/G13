#pragma once

#include "Event.h"

#include <vector>
#include <stddef.h>

class Window
{
public:
	~Window();

	typedef void (*DisplayCallback)();

	void create(bool fullscreen = true, int fsaa = 0);
	void title(const char *title);
	void vsync(bool enable);
	void size(int &width, int &height);
	void display(DisplayCallback callback);
	void push(const Event &event);
	bool poll(Event *event);
	void close();
	int rotation();

private:
	Window();
	void events();

	std::vector<Event> events_;
	size_t pollIndex_;

	friend class Application;
};
