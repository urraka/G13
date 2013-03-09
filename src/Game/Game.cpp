#include <pch.h>
#include <Graphics/OpenGL.h>
#include <Graphics/Graphics.h>
#include <System/Window.h>
#include <System/Events.h>
#include <System/Clock.h>
#include <Game/Game.h>

Game::Game()
	:	window(0),
		graphics(0),
		events(0)
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

	window = new Window();
	events = new Events();
	graphics = new Graphics();

	if (!window->init(false))
		return false;

	if (!graphics->init())
		return false;

	events->init();
	window->setTitle("G13");

	return true;
}

void Game::terminate()
{
	delete graphics;
	delete events;
	delete window;

	graphics = 0;
	events = 0;
	window = 0;
}

void Game::draw()
{
	graphics->clear();
	graphics->test(Clock::toSeconds<float>(time_ + timeAccumulator_));
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
				graphics->viewport(event.size.width, event.size.height);
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

void Game::loop()
{
	while (!quit_)
	{
		this->draw();
		this->input();
		this->update();
		window->display();
	}
}
