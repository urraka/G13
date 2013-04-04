#include <Game/Game.h>
#include <Game/Scenes/MainScene.h>
#include <Game/Map.h>

MainScene::MainScene(Game *game)
	:	Scene(game),
		map_(0),
		background_(0),
		textures_(),
		sprites_(0)
{
}

MainScene::~MainScene()
{
	delete map_;
	delete background_;
	delete sprites_;

	for (int i = 0; i < TextureCount; i++)
		delete textures_[i];
}

void MainScene::init()
{
	Graphics *graphics = game_->graphics;

	sprites_ = graphics->batch(1);
	textures_[TextureGuy] = graphics->texture("data/guy.png");
	sprites_->texture(textures_[TextureGuy]);

	int width, height;
	game_->window->size(width, height);
	background_ = graphics->buffer<ColorVertex>(VBO<ColorVertex>::TriangleFan, VBO<ColorVertex>::StaticDraw, 4);
	updateBackground(width, height);

	map_ = new Map();
	map_->load(graphics);

	camera_.target(&character_);
	camera_.viewport(width, height);
}

void MainScene::update(Time dt)
{
	if (Keyboard::pressed(Keyboard::NumpadAdd))
		camera_.zoom(Camera::ZoomIn);

	if (Keyboard::pressed(Keyboard::NumpadSubtract))
		camera_.zoom(Camera::ZoomOut);

	if (Keyboard::pressed(Keyboard::Left))
		character_.move(Character::MoveLeft);

	if (Keyboard::pressed(Keyboard::Right))
		character_.move(Character::MoveRight);

	if (Keyboard::pressed(Keyboard::Up))
		character_.move(Character::MoveUp);

	if (Keyboard::pressed(Keyboard::Down))
		character_.move(Character::MoveDown);

	character_.update(dt);
	camera_.update(dt);
}

void MainScene::draw(float framePercent)
{
	Graphics *graphics = game_->graphics;

	sprites_->clear();

	graphics->clear();
	graphics->bind(Graphics::ColorShader);
	graphics->matrix(mat4(1.0f));
	graphics->draw(background_);
	graphics->matrix(camera_.matrix(framePercent));
	map_->draw(graphics);
	character_.draw(sprites_, framePercent);
	graphics->draw(sprites_);
}

void MainScene::event(const Event &evt)
{
	switch (evt.type)
	{
		case Event::Resize:
		{
			game_->graphics->viewport(evt.resize.width, evt.resize.height, evt.resize.rotation);
			updateBackground(evt.resize.width, evt.resize.height);
			camera_.viewport(evt.resize.width, evt.resize.height);
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

void MainScene::updateBackground(int width, int height)
{
	ColorVertex vertices[4];

	vertices[0].position = vec2(0.0f, 0.0f);
	vertices[1].position = vec2((float)width, 0.0f);
	vertices[2].position = vec2((float)width, (float)height);
	vertices[3].position = vec2(0.0f, (float)height);

	vertices[0].color = u8vec4(0, 0, 255, 255);
	vertices[1].color = u8vec4(0, 0, 255, 255);
	vertices[2].color = u8vec4(255);
	vertices[3].color = u8vec4(255);

	background_->set(vertices, 0, 4);
}
