#include "Message.h"
#include "PlayerJoinMessage.h"
#include "../Server.h"
#include <assert.h>
#include <cstring>

namespace net
{
	bool PlayerJoinMessage::validate(const Message *msg)
	{
		assert(msg->type() == Message::Nick);

		if (msg->length < 3 || msg->length > Player::Info::MaxNickBytes + 2) // at least one byte of nickname, please
			return false;

		if (msg->data[1] >= Server::MaxPeers) // id out of range
			return false;

		// TODO: check if the string is valid UTF-8

		for (uint16_t i = 2; i < msg->length; i++)
		{
			if (msg->data[i] == 0) // no nulls inside nickname
				return false;
		}

		return true;
	}

	void PlayerJoinMessage::serialize(Message *msg)
	{
		nickname[sizeof(nickname) - 1] = 0;

		size_t len = strlen(nickname);

		data_[0] = Message::PlayerJoin;
		data_[1] = id;
		memcpy(&data_[2], nickname, len);

		msg->data = data_;
		msg->length = len + 2; // nick length with no null + msg type + player id

		assert(validate(msg));
	}

	void PlayerJoinMessage::unserialize(const Message *msg)
	{
		assert(validate(msg));

		id = msg->data[1];

		uint16_t len = msg->length - 2;
		memcpy(nickname, &msg->data[2], len);
		nickname[len] = 0;
	}
}
