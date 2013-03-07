#pragma once

#include <System/Event.h>
#include <vector>

class Events
{
public:
	Events();
	~Events();
	void init();
	void push(const Event &event);
	bool poll(Event *event);

private:
	std::vector<Event> events_;
	size_t pollIndex_;
};
