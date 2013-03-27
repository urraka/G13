#pragma once

#include <vector>

class Application;
class Window;

#include <Graphics/Graphics.h>

class Game {
public:
	Game();
	~Game();

	static void launch(Application *app);
	static void terminate();
	static void display();

	void init(Application *app);
	void draw();
	void input();
	void update();

	Window *window;
	Graphics *graphics;

private:
	uint64_t currentTime_;
	uint64_t timeAccumulator_;
	uint64_t dt_;
	uint64_t time_;
	uint64_t fpsTimer_;
	int fps_;

	// testing
	SpriteBatch *batch_;
	VBO<ColorVertex> *buffer_;
	Texture *texture_[2];
	std::vector<Sprite> sprites_;
	std::vector<float> spriteAngles_;
};
