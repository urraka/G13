#pragma once

class Game;

class Scene
{
public:
	Scene(Game *game) : game_(game) {}
	virtual ~Scene() {}

	virtual void init() = 0;
	virtual void update(uint64_t dt) = 0;
	virtual void draw(float percent) = 0;
	virtual void event(const Event &evt) = 0;

protected:
	Game *game_;
};
