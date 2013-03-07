#pragma once

class Event
{
public:
	enum EventType
	{
		None = 0,
		Close,
		Resize,
		KeyPress,
		KeyRelease,
		CharPress,
		CharRelease,
		MouseButtonPress,
		MouseButtonRelease
	};

	EventType type;

	union
	{
		struct { int width, height; } size;
		int button;
		int key;
		int character;
	};

	Event(EventType eventType = None, int param1 = 0, int param2 = 0)
		:	type(eventType)
	{
		switch (type)
		{
			case Close:
				break;

			case Resize:
				size.width = param1;
				size.height = param2;
				break;

			case KeyPress:
			case KeyRelease:
				key = param1;
				break;

			case CharPress:
			case CharRelease:
				character = param1;
				break;

			case MouseButtonPress:
			case MouseButtonRelease:
				button = param1;
				break;

			default:
				break;
		}
	}
};
