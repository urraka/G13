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
	void draw(const Frame &frame);
	bool event(Event *evt);

private:
	net::Client *client_;
	net::Server *server_;

	bool onKeyPressed(const Event::KeyEvent &key);
};

}} // g13::stt
