#pragma once

#include "State.h"

namespace net { class Client; }

namespace stt
{
	class Connecting : public State
	{
	public:
		Connecting();
		~Connecting();

		void update(Time dt);
		void draw(float percent);
		void event(const Event &evt);

	private:
		net::Client *client_;
	};
}
