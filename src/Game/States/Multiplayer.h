#pragma once

#include "State.h"
#include <sys/sys.h>

namespace net {
	class Client;
	class Server;
}

namespace stt {

class Multiplayer : public State
{
public:
	Multiplayer();
	~Multiplayer();

	void update(sys::Time dt);
	void draw(float percent);
	void event(sys::Event *evt);

private:
	net::Client *client_;
	net::Server *server_;

	void onKeyPressed(int key);
};

} // stt
