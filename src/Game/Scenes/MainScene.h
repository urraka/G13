#pragma once

#include <Game/Scene.h>
#include <vector>

class MainScene : public Scene
{
public:
	MainScene(Game *game);
	~MainScene();

	void init();
	void update(uint64_t dt);
	void draw(float percent);
	void event(const Event &evt);

	// testing
	uint64_t time_, prevTime_;
	SpriteBatch *batch_;
	VBO<MixedVertex> *buffer_;
	Texture *texture_[2];
	std::vector<Sprite> sprites_;
	std::vector<float> spriteAngles_;
};
