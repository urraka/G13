#include <System/System.h>
#include <Graphics/Graphics.h>
#include <Game/Game.h>
#include <Game/Scenes/MainScene.h>

namespace
{
	Game *game = 0;
}

void Game::launch(Application *app)
{
	game = new Game();
	game->init(app);
}

void Game::display()
{
	game->input();
	game->draw();
	game->update();
}

void Game::terminate()
{
	delete game;
}

Game::Game()
	:	graphics(0),
		window(0),
		scene_(0),
		dt_(0),
		currentTime_(0),
		timeAccumulator_(0),
		fps_(0),
		fpsTimer_(0)
{
}

Game::~Game()
{
	delete scene_;
	delete graphics;
}

void Game::init(Application *app)
{
	currentTime_ = Clock::time();
	dt_ = Clock::milliseconds(30);

	window = app->window(false);
	window->display(Game::display);
	window->title("G13");
	window->vsync(false);

	graphics = new Graphics();

	if (!graphics->init())
		return;

	ivec2 size;
	window->size(size.x, size.y);
	graphics->viewport(size.x, size.y, window->rotation());

	scene_ = new MainScene(this);
	scene_->init();
}

void Game::draw()
{
	scene_->draw((float)(timeAccumulator_ / (double)dt_));
	fps_++;
}

void Game::input()
{
	Event event;

	while (window->poll(&event))
	{
		scene_->event(event);
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
		scene_->update(dt_);
		timeAccumulator_ -= dt_;
	}
}
