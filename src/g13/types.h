#pragma once

namespace g13
{
	class Callback;
	class Frame;
	class Map;
	class Camera;

	namespace stt
	{
		class State;
		class Multiplayer;
		class MainMenu;
	}

	namespace net
	{
		class Peer;
		class Connection;
		class Client;
		class Server;
		class Player;
		class ServerPlayer;
		class RemotePlayer;
		class LocalPlayer;
		class Renderer;

		namespace msg
		{
			class Storage;
			class Message;
			class Login;
			class Pong;
			class JoinRequest;
			class Input;
			class ServerInfo;
			class PlayerInfo;
			class PlayerConnect;
			class PlayerDisconnect;
			class PlayerJoin;
			class PlayerLeave;
			class PlayerChat;
			class GameState;
			class Bullet;
			class Damage;
		}
	}

	namespace ent
	{
		class Soldier;
		class Bullet;
	}

	namespace cmp
	{
		class SoldierState;
		class SoldierInput;
		class SoldierPhysics;
		class SoldierGraphics;
		class BulletParams;
		class BulletPhysics;
		class BulletGraphics;
	}

	namespace coll
	{
		class Segment;
		class Hull;
		class Entity;
		class Result;
		class World;
	}

	namespace ui
	{
		class Bar;
	}
}

namespace Json
{
	class Value;
}
