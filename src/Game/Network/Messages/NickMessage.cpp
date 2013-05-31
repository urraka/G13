#include "Message.h"
#include "NickMessage.h"
#include <cstring>
#include <assert.h>

namespace net
{
	bool NickMessage::validate(const Message *msg)
	{
		assert(msg->type() == Message::Nick);

		if (msg->length < 2 || msg->length > Player::Info::MaxNickBytes + 1) // at least one byte of nickname, please
			return false;

		// TODO: check if the string is valid UTF-8

		for (uint16_t i = 1; i < msg->length; i++)
		{
			if (msg->data[i] == 0) // no nulls inside nickname
				return false;
		}

		return true;
	}

	void NickMessage::serialize(Message *msg)
	{
		nickname[sizeof(nickname) - 1] = 0; // so i can use good old strlen safely

		size_t len = strlen(nickname); // see?

		data_[0] = Message::Nick;
		memcpy(&data_[1], nickname, len);

		msg->data = data_;
		msg->length = len + 1; // +1 for null? no, for message type

		assert(validate(msg));
	}

	void NickMessage::unserialize(const Message *msg)
	{
		assert(validate(msg));

		uint16_t len = msg->length - 1;
		memcpy(nickname, &msg->data[1], len);
		nickname[len] = 0;
	}
}
