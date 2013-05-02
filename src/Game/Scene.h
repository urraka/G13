#pragma once

#include "../System/Clock.h"
#include "../System/Event.h"

class Scene
{
public:
	virtual ~Scene() {}

	virtual void init() = 0;
	virtual void update(Time dt) = 0;
	virtual void draw(float percent) = 0;
	virtual void event(const Event &evt) = 0;
};
