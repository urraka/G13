#include "Server.h"
#include "Messages/NickMessage.h"
#include "Messages/StartMessage.h"
#include "Messages/PlayerJoinMessage.h"
#include "Messages/PlayerLeaveMessage.h"
#include "Messages/SpawnMessage.h"
#include "Messages/GameStateMessage.h"
#include "../Debugger.h"

#include "../../Math/Math.h"

#include <iostream>
#include <algorithm>
#include <assert.h>

namespace net
{
	Server *server = 0;

	Server::Server()
		:	state_(Stopped),
			server_(0),
			tick_(0),
			dt_(Clock::milliseconds(30)),
			stopTimeout_(Clock::milliseconds(5000)),
			collisionMap_(0)
	{
		server = this;
		players_.reserve(MaxPeers);
	}

	Server::~Server()
	{
		if (server_ != 0)
			enet_host_destroy(server_);
	}

	void Server::start(int port)
	{
		assert(state_ == Stopped);

		ENetAddress address;
		address.host = ENET_HOST_ANY;
		address.port = port;

		server_ = enet_host_create(&address, MaxPeers, 2, 0, 0);

		if (server_ == 0)
			std::cerr << "Server: failed to start connection." << std::endl;

		players_.clear();

		for (int i = 0; i < MaxPeers; i++)
			playersBuffer_[i].id = i;

		map_.load();
		collisionMap_ = map_.collisionMap();

		tick_ = 0;
		time_ = Clock::time();
		state_ = Running;
	}

	void Server::stop(bool gracefully)
	{
		assert(state_ == Running);

		if (gracefully)
		{
			state_ = Stopping;

			for (size_t i = 0; i < players_.size(); i++)
				players_[i]->disconnect();

			stopTime_ = Clock::time();
		}
		else
		{
			enet_host_destroy(server_);

			server_ = 0;
			state_ = Stopped;

			for (size_t i = 0; i < players_.size(); i++)
				players_[i]->onDisconnect();
		}
	}

	void Server::update()
	{
		assert(state_ != Stopped);

		Time t = Clock::time();

		if (t - time_ < dt_)
			return;

		time_ = t;

		ENetEvent event;

		while (enet_host_service(server_, &event, 0) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					DBG( std::cout << "[Server] Peer connected." << std::endl );
					onPeerConnect(event.peer);
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					DBG( std::cout << "[Server] Peer disconnected." << std::endl );
					onPeerDisconnect(event.peer);
					break;

				case ENET_EVENT_TYPE_RECEIVE:
					onPacketReceived(event.peer, event.packet);
					enet_packet_destroy(event.packet);
					break;

				default: break;
			}
		}

		if (state_ == Stopping && (players_.size() == 0 || Clock::time() - stopTime_ > stopTimeout_))
		{
			stop(false);
		}
		else
		{
			std::vector<Player*>::iterator it;

			for (it = players_.begin(); it != players_.end();)
			{
				Player *player = *it;

				player->update(dt_);

				if (!player->connected())
					it = players_.erase(it);
				else
					++it;
			}
		}

		broadcast(Message::GameState, 0);

		tick_++;
	}

	bool Server::running() const
	{
		return state_ == Running;
	}

	uint32_t Server::tick() const
	{
		return tick_;
	}

	void Server::send(Message::Type messageType, Player *player)
	{
		assert(player != 0);

		switch (messageType)
		{
			case Message::Start:
			{
				Message msg;
				StartMessage startMessage;

				startMessage.nPlayers = 0;

				for (size_t i = 0; i < players_.size(); i++)
				{
					if (players_[i] != player && players_[i]->state() != Player::Joining && players_[i]->state() != Player::Disconnected)
					{
						startMessage.playersInfo[startMessage.nPlayers] = players_[i]->info();
						startMessage.nPlayers++;
					}
				}

				startMessage.serialize(&msg);

				ENetPacket *packet = enet_packet_create(msg.data, msg.length, ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(player->peer(), 0, packet);

				DBG( std::cout << "[Server] Sent StartMessage to player #" << player->id << "." << std::endl; );
			}
			break;

			default:
				assert(false);
				break;
		}
	}

	void Server::broadcast(Message::Type messageType, Player *player)
	{
		switch (messageType)
		{
			case Message::PlayerJoin:
			{
				assert(player != 0);

				Message msg;
				PlayerJoinMessage joinMessage;

				joinMessage.id = player->id;
				strncpy(joinMessage.nickname, player->nickname(), sizeof(joinMessage.nickname));

				joinMessage.serialize(&msg);

				ENetPacket *packet = enet_packet_create(msg.data, msg.length, ENET_PACKET_FLAG_RELIABLE);
				enet_host_broadcast(server_, 0, packet);

				DBG( std::cout << "[Server] Broadcasting PlayerJoinMessage." << std::endl; );
			}
			break;

			case Message::PlayerLeave:
			{
				assert(player != 0);

				Message msg;
				PlayerLeaveMessage leaveMessage;

				leaveMessage.id = player->id;

				leaveMessage.serialize(&msg);

				ENetPacket *packet = enet_packet_create(msg.data, msg.length, ENET_PACKET_FLAG_RELIABLE);
				enet_host_broadcast(server_, 0, packet);

				DBG( std::cout << "[Server] Broadcasting PlayerLeaveMessage." << std::endl; );
			}
			break;

			case Message::Spawn:
			{
				assert(player != 0);

				Message msg;
				SpawnMessage spawnMessage;

				spawnMessage.id = player->id;
				spawnMessage.tick = tick_;
				spawnMessage.position = player->position();

				spawnMessage.serialize(&msg);

				ENetPacket *packet = enet_packet_create(msg.data, msg.length, ENET_PACKET_FLAG_RELIABLE);
				enet_host_broadcast(server_, 0, packet);
			}
			break;

			case Message::GameState:
			{
				Message msg;
				GameStateMessage stateMessage;

				stateMessage.tick = tick_;
				stateMessage.nPlayers = 0;

				for (size_t i = 0; i < players_.size(); i++)
				{
					if (players_[i]->state() == Player::Alive)
					{
						int j = stateMessage.nPlayers;

						stateMessage.players[j].id = players_[i]->id;
						stateMessage.players[j].state = players_[i]->soldierState();
						stateMessage.nPlayers++;
					}
				}

				stateMessage.serialize(&msg);

				ENetPacket *packet = enet_packet_create(msg.data, msg.length, ENET_PACKET_FLAG_UNSEQUENCED);
				enet_host_broadcast(server_, 1, packet);
			}
			break;

			default:
				assert(false);
				break;
		}
	}

	void Server::onPeerConnect(ENetPeer *peer)
	{
		if (Player *player = findDisconnectedPlayer())
		{
			peer->data = player;
			player->onConnect(peer);
			players_.push_back(player);
		}
	}

	void Server::onPeerDisconnect(ENetPeer *peer)
	{
		Player *player = (Player*)peer->data;
		player->onDisconnect();

		std::vector<Player*>::iterator it = std::find(players_.begin(), players_.end(), player);
		players_.erase(it);

		// TODO: broadcast player disconnect
	}

	void Server::onPacketReceived(ENetPeer *peer, ENetPacket *packet)
	{
		Player *player = (Player*)peer->data;

		Message msg(packet->data, packet->dataLength);

		if (msg.validate())
			player->onMessage(&msg);
	}

	Player *Server::findDisconnectedPlayer()
	{
		for (int i = 0; i < MaxPeers; i++)
		{
			if (!playersBuffer_[i].connected())
				return &playersBuffer_[i];
		}

		return 0;
	}
}
