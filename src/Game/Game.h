#pragma once

class Window;
class Graphics;
class Events;
class Texture;

#include <Graphics/Graphics.h>

class Game {
public:
	Game();

	bool init();
	void terminate();
	void draw();
	void input();
	void update();
	void quit();
	void loop();

	Events *events;
	Graphics *graphics;
	ivec2 resolution; // TODO: choose better name

private:
	Window *window_;
	uint64_t currentTime_;
	uint64_t timeAccumulator_;
	uint64_t dt_;
	uint64_t time_;
	uint64_t fpsTimer_;
	int fps_;
	bool quit_;

	// testing
	Texture *texture_;
	std::vector<Sprite> sprites_;
	std::vector<float> spriteAngles_;
};

extern Game game;
