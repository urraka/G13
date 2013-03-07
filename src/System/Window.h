#pragma once

class Window
{
public:
	~Window();

	bool init(bool fullscreen = true);
	void setTitle(const char *title);

	void display();
};
