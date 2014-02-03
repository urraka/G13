#pragma once

#include <stdint.h>

namespace sys {

enum EventType
{
	Close,
	FocusChange,
	Resize,
	KeyPress,
	KeyRelease,
	KeyRepeat,
	Text,
	MouseButtonPress,
	MouseButtonRelease,
	MouseMove,
	MouseWheelMove,
	MouseEnter,
	MouseLeave
};

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

	struct FocusEvent
	{
		bool focused;
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
		FocusEvent       focus;
	};
};

} // sys
