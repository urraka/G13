#include "Player.h"
#include "Server.h"
#include "Messages/NickMessage.h"
#include "Messages/StartMessage.h"

namespace net
{
	Player::Player() : state_(Disconnected)
	{
		name_.reserve(Info::MaxNickBytes);
		reset();
	}

	Player::~Player()
	{
		reset();
	}

	void Player::update(Time dt)
	{
		if (state_ == Joining && (server->tick() - joinTick_) * dt >= Clock::milliseconds(5000))
		{
			reset();
			return;
		}
	}

	void Player::disconnect()
	{
		enet_peer_disconnect(peer_, 0);
	}

	bool Player::connected() const
	{
		return state_ != Disconnected;
	}

	void Player::onConnect(ENetPeer *peer)
	{
		peer_ = peer;
		state_ = Joining;
		joinTick_ = server->tick();

		name_.empty();
		soldier_.map(0);
		soldier_.reset(fixvec2(0, 0));
	}

	void Player::onDisconnect()
	{
		state_ = Disconnected;
		reset();
	}

	void Player::onMessage(const Message *msg)
	{
		switch (msg->type())
		{
			case Message::Nick:
			{
				if (state_ == Joining)
				{
					NickMessage nickMessage;
					nickMessage.unserialize(msg);

					name_ = nickMessage.nickname;

					server->send(Message::Start, this);
					server->broadcast(Message::PlayerJoin, this);
					state_ = Dead;
				}
			}
			break;

			case Message::SpawnRequest:
				break;

			case Message::Input:
				break;

			default: break;
		};
	}

	ENetPeer *Player::peer() const
	{
		return peer_;
	}

	Player::State Player::state() const
	{
		return state_;
	}

	Player::Info Player::info() const
	{
		Player::Info result;

		result.id = id;
		result.state = state_;
		result.flipped = soldier_.graphics.sprite.scale.x == -1.f;
		result.frame = soldier_.graphics.animation.frameIndex();
		result.position = soldier_.physics.position;
		result.velocity = soldier_.physics.velocity;

		strncpy(result.nickname, name_.c_str(), sizeof(result.nickname));
		result.nickname[sizeof(result.nickname) - 1] = 0;

		return result;
	}

	const char *Player::nickname() const
	{
		return name_.c_str();
	}

	void Player::reset()
	{
		if (connected())
			enet_peer_reset(peer_);

		peer_ = 0;
		state_ = Disconnected;
		name_.empty();
		joinTick_ = 0;
		baseTick_ = 0;
		soldier_.reset(fixvec2(0, 0));
	}
}
