#include "Message.h"
#include "GameStateMessage.h"
#include "../DataStream.h"

namespace net
{
	bool GameStateMessage::validate(const Message *msg)
	{
		assert(msg->type() == Message::GameState);

		if (msg->length < 5)
			return false;

		if ((msg->length - 5) % (1 + Player::SoldierState::Size) != 0)
			return false;

		int n = (msg->length - 5) / (1 + Player::SoldierState::Size);

		for (int i = 0; i < n; i++)
		{
			uint8_t id = msg->data[5 + (1 + Player::SoldierState::Size) * i];

			if (id >= Server::MaxPeers)
				return false;
		}

		return true;
	}

	void GameStateMessage::serialize(Message *msg)
	{
		assert(nPlayers < Server::MaxPeers);

		DataWriter w(data_, sizeof(data_));

		w << (uint8_t)Message::GameState;
		w << tick;

		for (int i = 0; i < nPlayers; i++)
		{
			w << players[i].id;
			w << players[i].state;
		}

		msg->data = data_;
		msg->length = w.tell();

		assert(validate(msg));
	}

	void GameStateMessage::unserialize(const Message *msg)
	{
		assert(validate(msg));

		nPlayers = (msg->length - 5) / (1 + Player::SoldierState::Size);

		DataReader r(msg->data, msg->length);

		r.seek(1, DataStream::Begin);
		r >> tick;

		for (int i = 0; i < nPlayers; i++)
		{
			r >> players[i].id;
			r >> players[i].state;
		}
	}
}
