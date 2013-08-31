#pragma once

namespace g13
{
	class Frame;
	class Map;
	class Collision;

	namespace stt
	{
		class State;
		class Multiplayer;
	}

	namespace net
	{
		class BitStream;
		class BitReader;
		class BitWriter;
		class Multiplayer;
		class Client;
		class Server;
		class Player;

		namespace msg
		{
			class Storage;
			class Message;
			class Chat;
			class Login;
			class Ready;
			class Input;
			class ServerInfo;
			class PlayerConnect;
			class PlayerDisconnect;
			class PlayerJoin;
			class GameState;
			class Bullet;
		}
	}

	namespace ent
	{
		class Camera;
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
}
