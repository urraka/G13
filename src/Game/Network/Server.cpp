#include "Server.h"
#include "../Debugger.h"

#include <iostream>
#include <assert.h>

Server::Server()
	:	state_(Stopped),
		server_(0),
		peers_(),
		peerCount_(0),
		stopTimeout_(Clock::milliseconds(5000))
{
}

Server::~Server()
{
	if (server_ != 0)
		enet_host_destroy(server_);
}

bool Server::start(int port)
{
	assert(state_ == Stopped);

	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = port;

	server_ = enet_host_create(&address, MaxPeers, 2, 0, 0);

	if (server_ == 0)
	{
		std::cerr << "Server: failed to start connection." << std::endl;
		return false;
	}

	peerCount_ = 0;
	memset(peers_, 0, sizeof(peers_));

	state_ = Running;

	return true;
}

void Server::stop()
{
	state_ = Stopping;

	for (int i = 0; i < peerCount_; i++)
		enet_peer_disconnect(peers_[i], 0);

	stopTime_ = Clock::time();
}

void Server::update()
{
	ENetEvent event;

	while (enet_host_service(server_, &event, 0) > 0)
	{
		switch (event.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
			{
				DBG( std::cout << "Server: incoming connection." << std::endl );

				int index = peerCount_;
				peers_[index] = event.peer;

				peerCount_++;
			}
			break;

			case ENET_EVENT_TYPE_DISCONNECT:
			{
				std::cout << "Server: client disconnected." << std::endl;

				int iPeer = -1;

				while (iPeer < (peerCount_ - 1) && peers_[++iPeer] != event.peer)
					;

				// TODO: do whatever's necessary when a peer disconnects

				for (int i = iPeer; i < peerCount_ - 1; i++)
					peers_[i] = peers_[i + 1];

				peers_[--peerCount_] = 0;
			}
			break;

			case ENET_EVENT_TYPE_RECEIVE:
			{
				// queue inputs received here, order them if necessary and ignore those behind current time

				enet_packet_destroy(event.packet);
			}
			break;

			default: break;
		}
	}

	if (state_ == Stopping && (peerCount_ == 0 || Clock::time() - stopTime_ > stopTimeout_))
	{
		for (int i = 0; i < peerCount_; i++)
			enet_peer_reset(peers_[i]);

		enet_host_destroy(server_);

		server_ = 0;
		state_ = Stopped;
	}
}
