#pragma once

#include "../Entities/Camera.h"
#include "../Entities/Soldier.h"
#include "../Map.h"
#include "../Replay.h"

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

	Replay replay_;
	Replay::Log replayLog_;
	Map map_;
	Camera camera_;
	Soldier soldier_;
	Sprite tree_;
	VBO<ColorVertex> *background_;
	Texture *textures_[TextureCount];
	SpriteBatch *sprites_;

	void updateBackground(int width, int height);
};
