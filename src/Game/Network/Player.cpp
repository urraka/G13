#include "Player.h"
#include "Server.h"
#include "Messages/NickMessage.h"
#include "Messages/StartMessage.h"
#include "Messages/InputMessage.h"

#include "../Debugger.h"

#include <iostream>

namespace net
{
	Player::Player() : state_(Disconnected)
	{
		inputs_.reserve(10);
		name_.reserve(Info::MaxNickBytes);
		reset();
	}

	Player::~Player()
	{
		reset();
	}

	void Player::update(Time dt)
	{
		switch (state_)
		{
			case Joining:
			{
				if ((server->tick() - joinTick_) * dt >= Clock::milliseconds(5000))
					reset();
			}
			break;

			case Alive:
			{
				for (size_t i = 0; i < inputs_.size(); i++)
				{
					cmp::SoldierInput input;
					input.unserialize(inputs_[i]);
					soldier_.update(dt, &input);
					lastTick_++;
				}
			}
			break;

			default: break;
		}

		inputs_.clear();
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
		if (state_ != Joining)
			server->broadcast(Message::PlayerLeave, this);

		state_ = Disconnected;
		reset();
	}

	void Player::onMessage(const Message *msg)
	{
		switch (msg->type())
		{
			case Message::Nick:
			{
				DBG( std::cout << "[Server] Received NickMessage from player #" << id << ". Nick: " << name_ << std::endl; );

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
			{
				if (state_ == Dead)
				{
					soldier_.reset(fixvec2(0, 0)); // TODO: retrieve spawn point
					state_ = Alive;
					lastTick_ = server->tick();

					server->broadcast(Message::Spawn, this);
				}
			}
			break;

			case Message::Input:
			{
				InputMessage inputMessage;
				inputMessage.unserialize(msg);

				if (state_ == Alive && inputMessage.tick > lastTick_ && inputMessage.tick <= server->tick())
					inputs_.push_back(inputMessage.input);
			}
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
		strncpy(result.nickname, name_.c_str(), sizeof(result.nickname));
		result.nickname[sizeof(result.nickname) - 1] = 0;
		result.soldierState = soldierState();

		return result;
	}

	Player::SoldierState Player::soldierState() const
	{
		SoldierState result;

		result.position  = soldier_.physics.position;
		result.velocity  = soldier_.physics.velocity;
		result.animation = soldier_.graphics.animation.id();
		result.frame     = soldier_.graphics.animation.frameIndex();
		result.flipped   = soldier_.graphics.sprite.scale.x == -1.f;

		return result;
	}

	const char *Player::nickname() const
	{
		return name_.c_str();
	}

	fixvec2 Player::position() const
	{
		return soldier_.physics.position;
	}

	void Player::reset()
	{
		if (connected())
			enet_peer_reset(peer_);

		peer_ = 0;
		state_ = Disconnected;
		name_.empty();
		joinTick_ = 0;
		lastTick_ = 0;
		soldier_.reset(fixvec2(0, 0));
	}
}

net::DataWriter& operator<<(net::DataWriter & s, net::Player::SoldierState const & state)
{
	uint16_t graphics = 0;

	graphics |= state.flipped ? (1 << 15) : 0;
	graphics |= (state.frame & 0x7F) << 8;
	graphics |= state.animation & 0xFF;

	s << state.position.x.value();
	s << state.position.y.value();
	s << state.velocity.x.value();
	s << state.velocity.y.value();
	s << graphics;

	return s;
}

net::DataReader& operator>>(net::DataReader & s, net::Player::SoldierState & state)
{
	int32_t px, py, vx, vy;
	uint16_t graphics;

	s >> px;
	s >> py;
	s >> vx;
	s >> vy;
	s >> graphics;

	state.position.x = fixed::from_value(px);
	state.position.y = fixed::from_value(py);
	state.velocity.x = fixed::from_value(vx);
	state.velocity.y = fixed::from_value(vy);
	state.flipped    = graphics & 0x8000;
	state.frame      = (graphics >> 8) & 0x7F;
	state.animation  = graphics & 0xFF;

	return s;
}
