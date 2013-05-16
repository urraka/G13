#include "Client.h"
#include "../Debugger.h"

#include <iostream>
#include <assert.h>

Client::Client()
	:	client_(0),
		peer_(0),
		state_(Disconnected)
{
}

Client::~Client()
{
	if (peer_)
		enet_peer_reset(peer_);

	if (client_)
		enet_host_destroy(client_);
}

bool Client::connect(const char *host, int port)
{
	assert(client_ == 0 && peer_ == 0);

	client_ = enet_host_create(0, 1, 2, 0, 0);

	if (client_ == 0)
	{
		std::cerr << "Client: enet_host_create failed." << std::endl;
		return false;
	}

	ENetAddress address;
	enet_address_set_host(&address, host);
	address.port = port;

	uint32_t     data          = 0;
	const size_t kChannelCount = 1;

	peer_ = enet_host_connect(client_, &address, kChannelCount, data);

	if (peer_ == 0)
	{
		std::cerr << "Client: enet_host_connect failed." << std::endl;
		enet_host_destroy(client_);
		client_ = 0;
		return false;
	}

	enet_peer_timeout(peer_, 0, 4000, 5000);

	state_ = Connecting;

	DBG( std::cout << "Client: connecting to " << host << ":" << port << "..." << std::endl );

	return true;
}

void Client::disconnect()
{
	if (client_ == 0 || peer_ == 0)
		return;

	enet_peer_disconnect(peer_, 0);
}

bool Client::connected() const
{
	return state_ == Connected;
}

bool Client::connecting() const
{
	return state_ == Connecting;
}

void Client::update()
{
	ENetEvent event;

	while (enet_host_service(client_, &event, 0) > 0)
	{
		switch (event.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
			{
				DBG( std::cout << "Client: connected to host." << std::endl );

				state_ = Connected;
			}
			break;

			case ENET_EVENT_TYPE_DISCONNECT:
			{
				DBG(
					if (state_ == Connecting)
						std::cout << "Client: connection failed." << std::endl;
					else
						std::cout << "Client: disconnected from host." << std::endl;
				);

				state_ = Disconnected;
			}
			break;

			case ENET_EVENT_TYPE_RECEIVE:
			{
				enet_packet_destroy(event.packet);
			}
			break;

			default: break;
		}
	}
}
