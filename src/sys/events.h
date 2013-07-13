#pragma once

namespace sys {

enum EventType { Keyboard, Mouse, Char, Resize };

struct Event
{
	Event(EventType t) : type(t) {}
	EventType type;
};

struct ResizeEvent : public Event
{
	ResizeEvent() : Event(Resize) {}
	int width;
	int height;
	int rotation;
};

struct KeyboardEvent : public Event
{
	KeyboardEvent() : Event(Keyboard) {}
	int  key;
	bool pressed;
};

struct CharEvent : public Event
{
	CharEvent() : Event(Char) {}
	int ch;
};

struct MouseEvent : public Event
{
	MouseEvent() : Event(Mouse) {}
	int  button;
	bool pressed;
};

} // sys
