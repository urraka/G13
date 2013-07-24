#pragma once

#include "Multiplayer.h"
#include <g13/g13.h>

namespace g13 {
namespace net {

class Server : public Multiplayer
{
public:
	enum State { Running, Stopping, Stopped };

	Server();

	bool start(int port);
	void stop();
	void update(Time dt);

	State state() const;

private:
	State state_;

	void onConnect   (ENetPeer *peer);
	void onDisconnect(ENetPeer *peer);
	void onMessage   (msg::Message *msg, ENetPeer *from);

	void onPlayerLogin(Player *player, msg::Message *msg);
	void onPlayerReady(Player *player, msg::Message *msg);
	void onPlayerInput(Player *player, msg::Message *msg);
};

}} // net