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

	void Player::info(Info *info)
	{
		info->id = id;
		info->state = state_;
		info->flipped = soldier_.graphics.sprite.scale.x == -1.f;
		info->frame = soldier_.graphics.animation.frameIndex();
		info->position = soldier_.physics.position;
		info->velocity = soldier_.physics.velocity;

		strncpy(info->nickname, name_.c_str(), sizeof(info->nickname));
		info->nickname[sizeof(info->nickname) - 1] = 0;
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
