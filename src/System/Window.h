#pragma once

#if !defined(IOS)
class Window
{
public:
	~Window();

	bool init(bool fullscreen = true);
	void title(const char *title);
	ivec2 size();
	void display();
};
#endif