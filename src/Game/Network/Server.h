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

		bool start(int port);
		void update(Time dt);

	private:
		State state_;

		void onConnect   (ENetPeer *peer);
		void onDisconnect(ENetPeer *peer);
		void onMessage   (msg::Message *msg, ENetPeer *from);

		void onPlayerLogin(Player *player, msg::Message *msg);
		void onPlayerReady(Player *player, msg::Message *msg);
		void onPlayerInput(Player *player, msg::Message *msg);
	};
}
