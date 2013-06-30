#pragma once

#include "State.h"

namespace net
{
	class Client;
	class Server;
}

namespace stt
{
	class Multiplayer : public State
	{
	public:
		Multiplayer();
		~Multiplayer();

		void update(Time dt);
		void draw(float percent);
		void event(const Event &evt);

	private:
		net::Client *client_;
		net::Server *server_;

		void onKeyPressed(Keyboard::Key key);
	};
}
