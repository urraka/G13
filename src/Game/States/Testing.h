#pragma once

#include "State.h"

#include "../../System/Clock.h"
#include "../../System/Event.h"
#include "../../Graphics/Graphics.h"
#include "../Entities/Camera.h"
#include "../Entities/Soldier.h"
#include "../Map.h"
#include "../Replay.h"

namespace net { class Client; }

namespace stt {

class Testing : public State
{
public:
	Testing(net::Client *client = 0);
	~Testing();

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
	ent::Camera camera_;
	ent::Soldier soldier_;
	Sprite tree_;
	VBO<ColorVertex> *background_;
	Texture *textures_[TextureCount];
	SpriteBatch *sprites_;
	net::Client *client_;

	void updateBackground(int width, int height);
};

} // stt
