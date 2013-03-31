#include <System/platform.h>
#include <System/Application.h>
#include <System/Window.h>
#include <System/Clock.h>
#include <Graphics/Graphics.h>
#include <Game/Game.h>

#include <stdint.h>
#include <string>
#include <iostream>
#include <fstream>

namespace
{
	Game *game = 0;
}

void Game::launch(Application *app)
{
	game = new Game();
	game->init(app);
}

void Game::terminate()
{
	delete game;
}

Game::Game()
	:	graphics(0),
		window(0),
		dt_(0),
		time_(0),
		currentTime_(0),
		timeAccumulator_(0),
		fps_(0),
		fpsTimer_(0),
		texture_(),
		buffer_(0),
		batch_(0)
{
}

Game::~Game()
{
	delete buffer_;
	delete texture_[0];
	delete texture_[1];
	delete graphics;
}

void Game::display()
{
	game->draw();
	game->input();
	game->update();
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
	graphics->bgcolor(0.5f, 0.5f, 0.5f);

	// testing stuff

	texture_[0] = graphics->texture("data/tree.png");
	texture_[1] = graphics->texture("data/white-tree.png");

	const size_t nSprites = 50;

	batch_ = graphics->batch(nSprites);
	sprites_.resize(nSprites);
	spriteAngles_.resize(nSprites);

	for (size_t i = 0; i < sprites_.size(); i++)
	{
		Sprite &sprite = sprites_[i];

		sprite.texcoords = vec4(0.0f, 0.0f, 1.0f, 1.0f);
		sprite.position = glm::diskRand(glm::min((float)size.x, (float)size.y) / 2.0f);
		sprite.center = vec2(168.0f, 252.0f);
		sprite.size = vec2((float)texture_[0]->width(), (float)texture_[0]->height());
		sprite.scale = vec2(0.5f, 0.5f);
		spriteAngles_[i] = glm::linearRand(-180.0f, 180.0f);
	}

	ColorVertex vertices[8];

	vertices[0].position = vec2(-128.0f, -128.0f);
	vertices[1].position = vec2(128.0f, -128.0f);
	vertices[2].position = vec2(128.0f, 128.0f);
	vertices[3].position = vec2(-128.0f, 128.0f);

	vertices[0].color = vec4(0.3f, 0.3f, 1.0f, 1.0f);
	vertices[1].color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	vertices[2].color = vec4(0.0f, 0.0f, 0.3f, 1.0f);
	vertices[3].color = vec4(0.0f, 0.0f, 0.3f, 1.0f);

	vertices[0].uv = vec2(0.0f, 0.0f);
	vertices[1].uv = vec2(1.0f, 0.0f);
	vertices[2].uv = vec2(1.0f, 1.0f);
	vertices[3].uv = vec2(0.0f, 1.0f);

	vertices[4].position = vec2(0.0f, 0.0f);
	vertices[5].position = vec2((float)size.x, 0.0f);
	vertices[6].position = vec2((float)size.x, (float)size.y);
	vertices[7].position = vec2(0.0f, (float)size.y);

	vertices[4].color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	vertices[5].color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	vertices[6].color = vec4(1.0f);
	vertices[7].color = vec4(1.0f);

	buffer_ = graphics->buffer<ColorVertex>(VBO<ColorVertex>::TriangleFan, VBO<ColorVertex>::StaticDraw, 8);
	buffer_->set(vertices, 0, 8);
}

void Game::draw()
{
	const float t = Clock::toSeconds<float>(time_ + timeAccumulator_);
	const float scale = glm::exp(1.0f * glm::sin(t));

	ivec2 size;
	window->size(size.x, size.y);

	graphics->clear();

	graphics->bind((Texture*)0);
	graphics->bind(Graphics::ColorShader);
	graphics->draw(buffer_, 4, 4);

	graphics->save();
	graphics->translate(size.x / 2.0f, size.y / 2.0f);
	graphics->scale(scale, scale);
	graphics->rotate(t * 45.0f);

	batch_->clear();

	for (size_t i = 0; i < sprites_.size(); i++)
	{
		sprites_[i].angle = spriteAngles_[i] + t * 90.0f;
		batch_->add(sprites_[i]);
	}

	graphics->bind(Graphics::TextureShader);
	graphics->bind(texture_[0]);
	graphics->draw(batch_);
	graphics->restore();

	graphics->save();
	graphics->translate(size.x - 128.0f, size.y - 128.0f);
	graphics->bind(Graphics::ColorShader);
	graphics->bind(texture_[1]);
	graphics->draw(buffer_, 0, 4);
	graphics->restore();


	fps_++;
}

void Game::input()
{
	Event event;

	while (window->poll(&event))
	{
		switch (event.type)
		{
			case Event::Resize:
			{
				graphics->viewport(event.resize.width, event.resize.height, event.resize.rotation);

				ColorVertex vertices[4];

				vertices[0].position = vec2(0.0f, 0.0f);
				vertices[1].position = vec2((float)event.resize.width, 0.0f);
				vertices[2].position = vec2((float)event.resize.width, (float)event.resize.height);
				vertices[3].position = vec2(0.0f, (float)event.resize.height);

				vertices[0].color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
				vertices[1].color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
				vertices[2].color = vec4(1.0f);
				vertices[3].color = vec4(1.0f);

				buffer_->set(vertices, 4, 4);

				break;
			}

			case Event::KeyPress:
				if (event.key == Keyboard::Escape)
					window->close();

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
