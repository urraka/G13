#pragma once

#include <enet/enet.h>

namespace net
{
	class Client
	{
	public:
		Client();
		~Client();

		bool connect(const char *host, int port);
		void disconnect();
		bool connected() const;
		bool connecting() const;
		void update();

	private:
		ENetHost *client_;
		ENetPeer *peer_;

		enum State
		{
			Disconnected,
			Connecting,
			Connected
		};

		State state_;
	};
}
