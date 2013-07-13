#pragma once

#include "State.h"

#include "../Entities/Camera.h"
#include "../Entities/Soldier.h"
#include "../Map.h"
#include "../Replay.h"

#include <sys/sys.h>
#include <gfx/forward.h>
#include <gfx/Sprite.h>

namespace net { class Client; }

namespace stt {

class Testing : public State
{
public:
	Testing(net::Client *client = 0);
	~Testing();

	void update(sys::Time dt);
	void draw(float framePercent);
	void event(sys::Event *event);

private:
	enum
	{
		TextureGuy = 0,
		TextureCount
	};

	Replay replay_;
	Replay::Log replayLog_;
	Map map_;
	ent::Camera camera_;
	ent::Soldier soldier_;

	gfx::VBO *background_;
	gfx::Texture *textures_[TextureCount];
	gfx::SpriteBatch *sprites_;

	net::Client *client_;

	void updateBackground(int width, int height);
};

} // stt
