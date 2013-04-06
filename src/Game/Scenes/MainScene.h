#pragma once

#include <Game/Entities/Camera.h>
#include <Game/Entities/Character.h>
#include <Game/Map.h>

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
		TextureTree,
		TextureCount
	};

	Map map_;
	Camera camera_;
	Character character_;
	Sprite tree_;
	VBO<ColorVertex> *background_;
	Texture *textures_[TextureCount];
	SpriteBatch *sprites_;

	void updateBackground(int width, int height);
};
