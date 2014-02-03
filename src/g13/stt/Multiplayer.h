#pragma once

#include <g13/g13.h>
#include <gfx/Color.h>
#include "State.h"

namespace g13 {
namespace stt {

class Multiplayer : public State
{
public:
	Multiplayer(const char *nickname, const gfx::Color &color, int port);
	Multiplayer(const char *nickname, const gfx::Color &color, const char *host, int port);
	~Multiplayer();

	void update(Time dt);
	void draw(const Frame &frame);
	bool onEvent(const sys::Event &event);

private:
	enum State { Connecting, Connected, Disconnecting };

	State state_;
	net::Client *client_;
	net::Server *server_;

	bool onKeyPressed(const sys::Event::KeyEvent &key);
};

}} // g13::stt
