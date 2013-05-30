#pragma once

#include "Player.h"
#include "Messages/Message.h"
#include "../../System/Clock.h"
#include "../Entities/Soldier.h"
#include "../Collision.h"
#include "../Map.h"

#include <enet/enet.h>
#include <string>

/**
 * Protocol draft:
 *   - Client: connects.
 *   - Server: receives connection, adds the client with state = Joining, waits for identification.
 *   - Client: identifies (nickname).
 *   - Server: when receiving identification, sends client its #ID and game state. Stores current tick as client base tick. Broadcasts join event. Sets client state = Dead/Idle.
 *   - Server: if identification is not received within a timeout, disconnects client.
 *   - Client: receives #ID and game state. Sets current tick = 0. Waits to have enough game state to interpolate data.
 *   - Server: periodically sends game state deltas.
 *   - Client: when enough game state is received (100ms?), starts simulation. Allows to request to spawn into the game.
 *   - Client: sends request to spawn into game.
 *   - Server: receives request and sends position where to spawn. Broadcasts player spawn event.
 *   - Client: receives the position and spawns. Starts playing and sending input data periodically. Input is sent along with a tick.
 *   - Server: when receiving input simulates the game and broadcasts game state deltas.
 *   - Server: periodically sends every client its server-side position to allow them to correct it.
 */

/**
 * Client -> Server packets:
 *   - NICK
 *   - SPAWN
 *   - INPUT
 *
 * Server -> Client packets:
 *   - START (#ID, map, player list)
 *   - JOIN
 *   - LEAVE
 *   - GAMESTATE
 *   - SPAWN
 */

// send initial data

// - Tick -> client will begin simulation on x tick, which would sync with this tick
// - Player id
// - Current players (skip players with Joining state)
//   - id
//   - state
//   - name
//   - position
//   - velocity
//   - animation frame/state?

namespace net
{
	class Server
	{
	public:
		Server();
		~Server();

		void start(int port);
		void stop(bool gracefully = true);
		void update();
		bool running() const;
		uint32_t tick() const;

		void send(Message::Type messageType, Player *player);
		void broadcast(Message::Type messageType, Player *player = 0);

		static const int MaxPeers = 32;

	private:
		enum State
		{
			Running,
			Stopping,
			Stopped
		};

		State state_;
		ENetHost *server_;

		uint32_t tick_;

		Time dt_;
		Time time_;
		Time stopTime_;
		Time stopTimeout_;

		Player playersBuffer_[MaxPeers];
		std::vector<Player*> players_;

		Map map_;
		const Collision::Map *collisionMap_;

		void onPeerConnect(ENetPeer *peer);
		void onPeerDisconnect(ENetPeer *peer);
		void onPacketReceived(ENetPeer *peer, ENetPacket *packet);

		Player *findDisconnectedPlayer();
	};

	extern Server *server;
}
