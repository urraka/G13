#pragma once

struct ResizeEvent
{
	int width;
	int height;
	int rotation;
};

struct KeyboardEvent
{
	Keyboard::Key key;
	bool pressed;
};

struct CharEvent
{
	int ch;
};

struct MouseEvent
{
	Mouse::Button button;
	bool pressed;
};

class Event
{
public:
	enum EventType
	{
		None = 0,
		Close,
		Resize,
		Keyboard,
		Char,
		Mouse
	};

	EventType type;

	union
	{
		MouseEvent mouse;
		ResizeEvent resize;
		CharEvent character;
		KeyboardEvent keyboard;
	};

	Event() : type(None) {}
	Event(EventType t) : type(t) {}
	Event(MouseEvent event) : type(Mouse), mouse(event) {}
	Event(ResizeEvent event) : type(Resize), resize(event) {}
	Event(CharEvent event) : type(Char), character(event) {}
	Event(KeyboardEvent event) : type(Keyboard), keyboard(event) {}
};
