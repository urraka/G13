#include <System/System.h>
#include <Graphics/Graphics.h>
#include <Game/Game.h>
#include <Game/Scenes/MainScene.h>

MainScene::MainScene(Game *game)
	:	Scene(game),
		time_(0),
		prevTime_(0),
		texture_(),
		buffer_(0),
		batch_(0)
{
}

MainScene::~MainScene()
{
	delete buffer_;
	delete texture_[0];
	delete texture_[1];
}

void MainScene::init()
{
	// testing stuff

	Graphics *graphics = game_->graphics;

	ivec2 size;
	game_->window->size(size.x, size.y);

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

	MixedVertex vertices[8];

	vertices[0].position = vec2(-128.0f, -128.0f);
	vertices[1].position = vec2(128.0f, -128.0f);
	vertices[2].position = vec2(128.0f, 128.0f);
	vertices[3].position = vec2(-128.0f, 128.0f);

	vertices[0].color = u8vec4(76, 76, 255, 255);
	vertices[1].color = u8vec4(0, 255, 0, 255);
	vertices[2].color = u8vec4(0, 0, 76, 255);
	vertices[3].color = u8vec4(0, 0, 76, 255);

	vertices[0].uv = vec2(0.0f, 0.0f);
	vertices[1].uv = vec2(1.0f, 0.0f);
	vertices[2].uv = vec2(1.0f, 1.0f);
	vertices[3].uv = vec2(0.0f, 1.0f);

	vertices[4].position = vec2(0.0f, 0.0f);
	vertices[5].position = vec2((float)size.x, 0.0f);
	vertices[6].position = vec2((float)size.x, (float)size.y);
	vertices[7].position = vec2(0.0f, (float)size.y);

	vertices[4].color = u8vec4(0, 0, 255, 255);
	vertices[5].color = u8vec4(0, 0, 255, 255);
	vertices[6].color = u8vec4(255);
	vertices[7].color = u8vec4(255);

	buffer_ = graphics->buffer<MixedVertex>(VBO<MixedVertex>::TriangleFan, VBO<MixedVertex>::StaticDraw, 8);
	buffer_->set(vertices, 0, 8);
}

void MainScene::update(uint64_t dt)
{
	prevTime_ = time_;
	time_ += dt;
}

void MainScene::draw(float percent)
{
	Graphics *graphics = game_->graphics;

	const float t = Clock::toSeconds<float>(time_ + uint64_t((time_ - prevTime_) * percent));
	const float scale = glm::exp(1.0f * glm::sin(t));

	ivec2 size;
	game_->window->size(size.x, size.y);

	graphics->clear();

	graphics->bind((Texture*)0);
	graphics->bind(Graphics::MixedShader);
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
	graphics->bind(Graphics::MixedShader);
	graphics->bind(texture_[1]);
	graphics->draw(buffer_, 0, 4);
	graphics->restore();
}

void MainScene::event(const Event &evt)
{
	switch (evt.type)
	{
		case Event::Resize:
		{
			game_->graphics->viewport(evt.resize.width, evt.resize.height, evt.resize.rotation);

			MixedVertex vertices[4];

			vertices[0].position = vec2(0.0f, 0.0f);
			vertices[1].position = vec2((float)evt.resize.width, 0.0f);
			vertices[2].position = vec2((float)evt.resize.width, (float)evt.resize.height);
			vertices[3].position = vec2(0.0f, (float)evt.resize.height);

			vertices[0].color = u8vec4(0, 0, 255, 255);
			vertices[1].color = u8vec4(0, 0, 255, 255);
			vertices[2].color = u8vec4(255);
			vertices[3].color = u8vec4(255);

			buffer_->set(vertices, 4, 4);

			break;
		}

		case Event::Keyboard:
		{
			if (evt.keyboard.pressed && evt.keyboard.key == Keyboard::Escape)
				game_->window->close();

			break;
		}

		default:
			break;
	}
}
