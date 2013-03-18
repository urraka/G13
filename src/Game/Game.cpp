#include <pch.h>
#include <Graphics/OpenGL.h>
#include <Graphics/Graphics.h>
#include <System/Window.h>
#include <System/Events.h>
#include <System/Clock.h>
#include <Game/Game.h>

#include <string>
#include <iostream>
#include <fstream>

Game::Game()
	:	graphics(0),
		events(0),
		window_(0),
		texture_(0)
{
}

bool Game::init()
{
	quit_ = false;
	timeAccumulator_ = 0;
	currentTime_ = Clock::time();
	dt_ = Clock::milliseconds(30);
	time_ = 0;
	fpsTimer_ = 0;
	fps_ = 0;

	events = new Events();
	graphics = new Graphics();

	// iOS already has the view and OpenGL context created by now
	#if !defined(IOS)
		window_ = new Window();

		if (!window_->init(false))
			return false;

		window_->title("G13");
		resolution = window_->size();
	#endif

	events->init();

	if (!graphics->init())
		return false;

	graphics->viewport(resolution.x, resolution.y);
	graphics->background(0.5f, 0.5f, 0.5f);

	texture_ = new Texture();
	texture_->load("data/tree.png");
	graphics->texture(texture_);

	return true;
}

void Game::terminate()
{
	delete texture_;
	delete graphics;
	delete events;

	#if !defined(IOS)
		delete window_;
	#endif

	graphics = 0;
	events = 0;
	window_ = 0;
}

void Game::draw()
{
	graphics->save();
	graphics->clear();
	graphics->translate(-128.0f, -128.0f);
	graphics->rotate(45.0f);
	graphics->translate(resolution.x / 2.0f, resolution.y / 2.0f);
	graphics->draw();
	graphics->restore();

	fps_++;
}

void Game::input()
{
	Event event;

	while (events->poll(&event))
	{
		switch (event.type)
		{
			case Event::Close:
				this->quit();
				break;

			case Event::Resize:
				resolution = ivec2(event.size.width, event.size.height);
				graphics->viewport(resolution.x, resolution.y);
				break;

			case Event::KeyPress:
				if (event.key == Keyboard::Escape)
					this->quit();

				break;

			default:
				break;
		}
	}
}

void Game::update()
{
	const uint64_t maxFrameTime = Clock::milliseconds(250);

	uint64_t newTime = Clock::time();
	uint64_t frameTime = newTime - currentTime_;

	fpsTimer_ += frameTime;

	if (fpsTimer_ >= Clock::seconds(1))
	{
		std::cout << "FPS: " << fps_ << " - Frame time: " << frameTime << std::endl;

		fpsTimer_ -= Clock::seconds(1);
		fps_ = 0;
	}

	if (frameTime > maxFrameTime)
		frameTime = maxFrameTime;

	currentTime_ = newTime;
	timeAccumulator_ += frameTime;

	while (timeAccumulator_ >= dt_)
	{
		// TODO: Update logic

		time_ += dt_;
		timeAccumulator_ -= dt_;
	}

	// TODO: interpolate
}

void Game::quit()
{
	quit_ = true;
}

#if !defined(IOS)
	// no loop in iOS, instead draw, input, update will be called by a callback hooked up in ios.mm
	void Game::loop()
	{
		while (!quit_)
		{
			this->draw();
			this->input();
			this->update();
			window_->display();
		}
	}
#endif
