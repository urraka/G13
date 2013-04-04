#pragma once

class MainScene : public Scene
{
public:
	MainScene(Game *game);
	~MainScene();

	void init();
	void update(Time dt);
	void draw(float percent);
	void event(const Event &evt);

	// testing
	Time time_;
	Time prevTime_;
	SpriteBatch *batch_;
	VBO<MixedVertex> *buffer_;
	Texture *texture_[2];
	std::vector<Sprite> sprites_;
	std::vector<float> spriteAngles_;
};
