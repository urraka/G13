#pragma once

#include "State.h"

#include "../../System/Clock.h"
#include "../../System/Event.h"
#include "../../Graphics/Graphics.h"
#include "../Entities/Camera.h"
#include "../Entities/Soldier.h"
#include "../Map.h"
#include "../Replay.h"

namespace stt
{
	class Testing : public State
	{
	public:
		Testing();
		~Testing();

		void update(Time dt);
		void draw(float framePercent);
		void event(const Event &evt);

		stt_name_decl();

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
}
