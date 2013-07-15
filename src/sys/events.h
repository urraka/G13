#pragma once

#include <stdint.h>

namespace sys {

class Event
{
public:
	struct SizeEvent
	{
		int width;
		int height;
		int fboWidth;
		int fboHeight;
		int rotation;
	};

	struct KeyEvent
	{
		int  code;
		int  scancode;
		bool shift;
		bool ctrl;
		bool alt;
		bool super;
	};

	struct TextEvent
	{
		uint32_t ch;
	};

	struct MouseButtonEvent
	{
		int  code;
		bool shift;
		bool ctrl;
		bool alt;
		bool super;
	};

	struct MouseMoveEvent
	{
		double x;
		double y;
	};

	struct MouseWheelEvent
	{
		double xoffset;
		double yoffset;
	};

	enum EventType
	{
		FocusGained,
		FocusLost,
		Resized,
		KeyPressed,
		KeyReleased,
		KeyRepeat,
		TextEntered,
		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseWheelMoved,
		MouseEntered,
		MouseLeft
	};

	EventType type;

	union
	{
		SizeEvent        size;
		KeyEvent         key;
		TextEvent        text;
		MouseButtonEvent mouseButton;
		MouseMoveEvent   mouseMove;
		MouseWheelEvent  mouseWheel;
	};
};

} // sys
