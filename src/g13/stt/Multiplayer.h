#pragma once

#include <g13/g13.h>
#include "State.h"

namespace g13 {
namespace stt {

class Multiplayer : public State
{
public:
	Multiplayer(const string32_t &name, int port);
	Multiplayer(const string32_t &name, const char *host, int port);
	~Multiplayer();

	void update(Time dt);
	void draw(const Frame &frame);
	bool event(Event *evt);

private:
	enum State { Connecting, Connected, Disconnecting };

	State state_;
	net::Client *client_;
	net::Server *server_;

	bool onKeyPressed(const Event::KeyEvent &key);
};

}} // g13::stt
