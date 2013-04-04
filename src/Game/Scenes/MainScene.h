#pragma once

#include <Game/Entities/Camera.h>
#include <Game/Entities/Character.h>

class Map;

class MainScene : public Scene
{
public:
	MainScene(Game *game);
	~MainScene();

	void init();
	void update(Time dt);
	void draw(float framePercent);
	void event(const Event &evt);

private:
	enum
	{
		TextureGuy = 0,
		TextureCount
	};

	Map *map_;
	VBO<ColorVertex> *background_;
	Camera camera_;
	Character character_;
	Texture *textures_[TextureCount];
	SpriteBatch *sprites_;

	void updateBackground(int width, int height);
};
