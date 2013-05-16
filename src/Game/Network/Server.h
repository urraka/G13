#pragma once

#include "../../System/Clock.h"
#include <enet/enet.h>

class Server
{
public:
	Server();
	~Server();

	bool start(int port);
	void stop();
	void update();
	bool running() const;


private:
	static const int MaxPeers = 32;

	enum State
	{
		Running,
		Stopping,
		Stopped
	};

	State state_;
	ENetHost *server_;
	ENetPeer *peers_[MaxPeers];
	int peerCount_;

	Time stopTime_;
	Time stopTimeout_;
};
