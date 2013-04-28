#include <Game/Game.h>
#include <Game/Scenes/MainScene.h>

MainScene::MainScene()
	:	background_(0),
		textures_(),
		sprites_(0)
{
}

MainScene::~MainScene()
{
	delete background_;
	delete sprites_;

	for (int i = 0; i < TextureCount; i++)
		delete textures_[i];
}

void MainScene::init()
{
	Graphics *graphics = game->graphics;

	sprites_ = graphics->batch(1);
	textures_[TextureGuy] = graphics->texture("data/guy.png");
	textures_[TextureTree] = graphics->texture("data/tree.png");

	int width, height;
	game->window->size(width, height);
	background_ = graphics->buffer<ColorVertex>(vbo_t::TriangleFan, vbo_t::StaticDraw, 4);
	updateBackground(width, height);

	map_.load();
	soldier_.map(map_.collisionMap());
	soldier_.spawn(vec2(150.0f, -500.0f));
	camera_.target(&soldier_);
	camera_.viewport(width, height);

	tree_.position = vec2(0.0f, -100.0f);
	tree_.center = vec2(128.0f, 250.0f);
	tree_.size = vec2(256.0f, 256.0f);
	tree_.texcoords = vec4(0.0f, 0.0f, 1.0f, 1.0f);
}

void MainScene::update(Time dt)
{
	if (Keyboard::pressed(Keyboard::NumpadAdd))
		camera_.zoom(Camera::ZoomIn);

	if (Keyboard::pressed(Keyboard::NumpadSubtract))
		camera_.zoom(Camera::ZoomOut);

	soldier_.update(dt);
	camera_.update(dt);
}

void MainScene::draw(float framePercent)
{
	Graphics *graphics = game->graphics;

	graphics->clear();

	graphics->bind(Graphics::ColorShader);
	graphics->draw(background_);

	graphics->save();
	graphics->matrix(camera_.matrix(framePercent));

	// sprites_->clear();
	// sprites_->add(tree_);
	// sprites_->texture(textures_[TextureTree]);
	// graphics->draw(sprites_);

	map_.draw(graphics);

	sprites_->clear();
	soldier_.draw(sprites_, framePercent);
	sprites_->texture(textures_[TextureGuy]);
	graphics->draw(sprites_);

	graphics->restore();
}

void MainScene::event(const Event &evt)
{
	switch (evt.type)
	{
		case Event::Resize:
		{
			updateBackground(evt.resize.width, evt.resize.height);
			camera_.viewport(evt.resize.width, evt.resize.height);
			break;
		}

		case Event::Keyboard:
		{
			if (evt.keyboard.pressed)
			{
				if (evt.keyboard.key == Keyboard::Escape)
				{
					//soldier_.saveInput("G13.replay");
					game->window->close();
				}
				else if (evt.keyboard.key == Keyboard::Enter)
				{
					soldier_.spawn(vec2(150.0f, -500.0f));
					camera_.target(&soldier_);
					soldier_.replay("G13.replay");
				}
			}

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
	vertices[2].color = u8vec4(255, 255, 255, 255);
	vertices[3].color = u8vec4(255, 255, 255, 255);

	background_->set(vertices, 0, 4);
}
