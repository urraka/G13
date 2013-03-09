#include <pch.h>
#if !defined(IOS)
	#include <GL/glfw.h>
#endif
#include <System/Events.h>

#if !defined(IOS)
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
#endif

Events::Events()
	:	pollIndex_(0)
{
}

Events::~Events()
{
	#if !defined(IOS)
		glfwSetWindowCloseCallback(0);
		glfwSetWindowSizeCallback(0);
		glfwSetKeyCallback(0);
		glfwSetCharCallback(0);
		glfwSetMouseButtonCallback(0);
	#endif
}

void Events::init()
{
	events_.reserve(20); // TODO: choose a good number

	#if !defined(IOS)
		callbacks::events = this;
		glfwDisable(GLFW_AUTO_POLL_EVENTS);
		glfwSetWindowCloseCallback(callbacks::close);
		glfwSetWindowSizeCallback(callbacks::resize);
		glfwSetKeyCallback(callbacks::keyboard);
		glfwSetCharCallback(callbacks::character);
		glfwSetMouseButtonCallback(callbacks::mouse);
	#endif
}

void Events::push(const Event &event)
{
	events_.push_back(event);
}

bool Events::poll(Event *event)
{
	#if !defined(IOS)
		if (pollIndex_ == 0)
			glfwPollEvents();
	#endif

	if (pollIndex_ == events_.size())
	{
		events_.clear();
		pollIndex_ = 0;
		return false;
	}

	*event = events_[pollIndex_++];
	return true;
}
