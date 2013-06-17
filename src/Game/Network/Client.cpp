#include "Client.h"
#include "Messages/Message.h"
#include "Messages/StartMessage.h"
#include "Messages/SpawnMessage.h"
#include "Messages/PlayerJoinMessage.h"
#include "Messages/PlayerLeaveMessage.h"
#include "Messages/GameStateMessage.h"
#include "Messages/NickMessage.h"
#include "Messages/InputMessage.h"

#include "../Debugger.h"

#include <iostream>
#include <assert.h>

namespace net
{
	Client::Client()
		:	client_(0),
			peer_(0),
			state_(Disconnected),
			playerState_(Player::Disconnected)
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
			std::cerr << "[Client] enet_host_create failed." << std::endl;
			return false;
		}

		ENetAddress address;
		enet_address_set_host(&address, host);
		address.port = port;

		peer_ = enet_host_connect(client_, &address, 2, 0);

		if (peer_ == 0)
		{
			std::cerr << "[Client] enet_host_connect failed." << std::endl;
			enet_host_destroy(client_);
			client_ = 0;
			return false;
		}

		enet_peer_timeout(peer_, 0, 4000, 5000);

		state_ = Connecting;

		DBG( std::cout << "[Client] Connecting to " << host << ":" << port << "..." << std::endl );

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
					DBG( std::cout << "[Client] Connected to host." << std::endl );

					state_ = Connected;
					playerState_ = Player::Joining;

					Message msg;
					NickMessage nickMessage;

					strncpy(nickMessage.nickname, "player", sizeof(nickMessage.nickname));

					nickMessage.serialize(&msg);

					ENetPacket *packet = enet_packet_create(msg.data, msg.length, ENET_PACKET_FLAG_RELIABLE);
					enet_host_broadcast(client_, 0, packet);
				}
				break;

				case ENET_EVENT_TYPE_DISCONNECT:
				{
					DBG(
						if (state_ == Connecting)
							std::cout << "[Client] Connection failed." << std::endl;
						else
							std::cout << "[Client] Disconnected from host." << std::endl;
					);

					state_ = Disconnected;
				}
				break;

				case ENET_EVENT_TYPE_RECEIVE:
				{
					onPacketReceived(event.packet);
					enet_packet_destroy(event.packet);
				}
				break;

				default: break;
			}
		}
	}

	void Client::onPacketReceived(ENetPacket *packet)
	{
		Message msg(packet->data, packet->dataLength);

		if (!msg.validate())
		{
			DBG( std::cout << "[Client] Invalid message with type = " << (int)msg.type() << std::endl; );
			return;
		}

		switch (msg.type())
		{
			case Message::Start:
			{
				DBG( std::cout << "[Client] Received StartMessage." << std::endl; );
			}
			break;

			case Message::Spawn:
			{
				DBG( std::cout << "[Client] Received SpawnMessage." << std::endl; );
			}
			break;

			case Message::PlayerJoin:
			{
				DBG( std::cout << "[Client] Received PlayerJoinMessage." << std::endl; );
			}
			break;

			case Message::PlayerLeave:
			{
				DBG( std::cout << "[Client] Received PlayerLeaveMessage." << std::endl; );
			}
			break;

			case Message::GameState:
			{
			}
			break;

			default: break;
		}
	}
}
