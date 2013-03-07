#include <pch.h>
#include <GL/glfw.h>
#include <System/Events.h>

namespace
{
namespace callbacks
{
	Events *events = 0;

	int GLFWCALL close()
	{
		events->push(Event(Event::Close));
		return GL_TRUE;
	}

	void GLFWCALL resize(int width, int height) { events->push(Event(Event::Resize, width, height)); }
	void GLFWCALL keyboard(int key, int action) { events->push(Event(action == GLFW_PRESS ? Event::KeyPress : Event::KeyRelease, key)); }
	void GLFWCALL character(int ch, int action) { events->push(Event(action == GLFW_PRESS ? Event::CharPress : Event::CharRelease, ch)); }
	void GLFWCALL mouse(int button, int action) { events->push(Event(action == GLFW_PRESS ? Event::MouseButtonPress : Event::MouseButtonRelease, button)); }
}
}

Events::Events()
	:	pollIndex_(0)
{
}

Events::~Events()
{
	glfwSetWindowCloseCallback(0);
	glfwSetWindowSizeCallback(0);
	glfwSetKeyCallback(0);
	glfwSetCharCallback(0);
	glfwSetMouseButtonCallback(0);
}

void Events::init()
{
	callbacks::events = this;

	events_.reserve(20); // TODO: choose a good number

	glfwDisable(GLFW_AUTO_POLL_EVENTS);

	glfwSetWindowCloseCallback(callbacks::close);
	glfwSetWindowSizeCallback(callbacks::resize);
	glfwSetKeyCallback(callbacks::keyboard);
	glfwSetCharCallback(callbacks::character);
	glfwSetMouseButtonCallback(callbacks::mouse);
}

void Events::push(const Event &event)
{
	events_.push_back(event);
}

bool Events::poll(Event *event)
{
	if (pollIndex_ == 0)
		glfwPollEvents();

	if (pollIndex_ == events_.size())
	{
		events_.clear();
		pollIndex_ = 0;
		return false;
	}

	*event = events_[pollIndex_++];
	return true;
}
