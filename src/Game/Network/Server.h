#pragma once

#include "Multiplayer.h"

namespace net
{
	class Server : public Multiplayer
	{
	public:
		enum State { Running, Stopping, Stopped };

		Server();
		~Server();

		void start(int port);
		void update(Time dt);

	private:
		State state_;

		void onConnect   (ENetPeer *peer);
		void onDisconnect(ENetPeer *peer);
		void onMessage   (msg::Message *msg, ENetPeer *from);
	};
}
