#pragma once

#include "State.h"

namespace g13 {
namespace net {
	class Client;
	class Server;
}}

namespace g13 {
namespace stt {

class Multiplayer : public State
{
public:
	Multiplayer();
	~Multiplayer();

	void update(Time dt);
	void draw(float percent);
	bool event(Event *evt);

private:
	net::Client *client_;
	net::Server *server_;

	bool onKeyPressed(int key);
};

}} // g13::stt
