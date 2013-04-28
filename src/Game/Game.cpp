#include <Game/Game.h>
#include <Game/Scenes/MainScene.h>

#define DEBUG_FPS 0

Game *game = 0;

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
	:	window(0),
		graphics(0),
		scene_(0),
		currentTime_(0),
		timeAccumulator_(0),
		dt_(0),
		fpsTimer_(0),
		fps_(0),
		tick_(0),
		stepMode_(false)
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

	window = app->window(false, 0);
	window->display(Game::display);
	window->title("G13");
	window->vsync(false);

	graphics = new Graphics();
	graphics->init();

	ivec2 size;
	window->size(size.x, size.y);
	graphics->viewport(size.x, size.y, window->rotation());

	scene_ = new MainScene();
	scene_->init();
}

void Game::draw()
{
	float percent = (float)(timeAccumulator_ / (double)dt_);

	#ifdef DEBUG
		if (stepMode_) percent = 1.0f;
	#endif

	scene_->draw(percent);
	fps_++;
}

void Game::input()
{
	Event event;

	while (window->poll(&event))
	{
		scene_->event(event);

		if (event.type == Event::Resize)
			graphics->viewport(event.resize.width, event.resize.height, event.resize.rotation);

		#ifdef DEBUG
			if (event.type == Event::Keyboard)
			{
				if (event.keyboard.pressed)
				{
					if (event.keyboard.key == Keyboard::F5)
						stepMode_ = !stepMode_;

					if (stepMode_ && event.keyboard.key == Keyboard::F10)
					{
						scene_->update(dt_);
						tick_++;
					}
				}
			}
		#endif
	}
}

void Game::update()
{
	#ifdef DEBUG
		if (stepMode_) return;
	#endif

	const Time maxFrameTime = Clock::milliseconds(250);

	Time newTime = Clock::time();
	Time frameTime = newTime - currentTime_;

	#if defined(DEBUG) && DEBUG_FPS
		fpsTimer_ += frameTime;

		if (fpsTimer_ >= Clock::seconds(1))
		{
			std::cout << "FPS: " << fps_ << " - Frame time: " << frameTime << std::endl;

			fpsTimer_ -= Clock::seconds(1);
			fps_ = 0;
		}
	#endif

	if (frameTime > maxFrameTime)
		frameTime = maxFrameTime;

	currentTime_ = newTime;
	timeAccumulator_ += frameTime;

	while (timeAccumulator_ >= dt_)
	{
		scene_->update(dt_);
		timeAccumulator_ -= dt_;
		tick_++;
	}
}
