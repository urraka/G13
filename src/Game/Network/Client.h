#pragma once

#include "Multiplayer.h"
#include <string>

namespace net
{
	class Client : public Multiplayer
	{
	public:
		enum State { Disconnected, Connecting, Connected };

		Client();
		~Client();

		bool connect(const char *host, int port);
		void update(Time dt);

		bool active() const;

	private:
		State state_;
		ENetPeer *peer_;

		uint8_t id_;
		char name_[Player::MaxNameLength + 1];
		cmp::SoldierInput input_;

		int connectingCount_;

		void onConnect   (ENetPeer *peer);
		void onDisconnect(ENetPeer *peer);
		void onMessage   (msg::Message *msg, ENetPeer *from);

		void onServerInfo(msg::Message *msg);
		void onPlayerConnect(msg::Message *msg);
		void onPlayerDisconnect(msg::Message *msg);
		void onPlayerJoin(msg::Message *msg);
		void onGameState(msg::Message *msg);
	};
}
