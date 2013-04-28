#pragma once

#include <Game/Entities/Camera.h>
#include <Game/Entities/Soldier.h>
#include <Game/Map.h>

class MainScene : public Scene
{
public:
	MainScene();
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
	Soldier soldier_;
	Sprite tree_;
	VBO<ColorVertex> *background_;
	Texture *textures_[TextureCount];
	SpriteBatch *sprites_;

	void updateBackground(int width, int height);
};
