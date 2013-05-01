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
	soldier_.reset(fixvec2(150, -500));

	camera_.target(&soldier_);
	camera_.viewport(width, height);
}

void MainScene::update(Time dt)
{
	if (replay_.tick() == 85)
		assert(replay_.tick() > 1);

	replayLog_.update(&replay_, &soldier_);

	if (Keyboard::pressed(Keyboard::NumpadAdd))
		camera_.zoom(Camera::ZoomIn);

	if (Keyboard::pressed(Keyboard::NumpadSubtract))
		camera_.zoom(Camera::ZoomOut);

	soldier_.update(dt, &replay_);
	camera_.update(dt);

	if (replay_.state() == Replay::Recording)
		replay_.input(&soldier_.input);
}

void MainScene::draw(float framePercent)
{
	Graphics *graphics = game->graphics;

	graphics->clear();

	graphics->bind(Graphics::ColorShader);
	graphics->draw(background_);

	graphics->save();
	graphics->matrix(camera_.matrix(framePercent));

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
				switch (evt.keyboard.key)
				{
					case Keyboard::Escape:
					{
						game->window->close();
					}
					break;

					case Keyboard::F11:
					{
						if (replay_.state() == Replay::Idle)
							replay_.startRecording(&soldier_);
						else if (replay_.state() == Replay::Recording)
							replay_.stopRecording("G13.replay");
					}
					break;

					case Keyboard::F12:
					{
						if (replay_.state() == Replay::Idle)
						{
							replay_.play("G13.replay", &soldier_);
							camera_.target(&soldier_);
						}
						else if (replay_.state() == Replay::Playing)
						{
							replay_.stop();
							replayLog_.save("replay.log");
						}
					}
					break;

					default:
						break;
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
