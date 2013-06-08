#include "Message.h"
#include "StartMessage.h"
#include "../DataStream.h"
#include <cstring>
#include <assert.h>

namespace net
{
	bool StartMessage::validate(const Message *msg)
	{
		assert(msg->type() == Message::Start);

		if (msg->length == 1) // no players on the list, could happen if you join first
			return true;

		if (msg->length > sizeof(StartMessage::data_))
			return false;

		DataReader reader(msg->data, msg->length);
		reader.seek(1, DataStream::Begin);

		while (reader.tell() < msg->length)
		{
			uint8_t idAndState;
			reader >> idAndState;

			if ((idAndState >> 4) >= Server::MaxPeers) // validate id
				return false;

			if ((idAndState & 0xF) >= Player::StateCount) // validate state
				return false;

			if (reader.tell() >= msg->length) // no more bytes? bad
				return false;

			char str[Player::Info::MaxNickBytes + 1]; // null terminated

			for (int i = 0; i < Player::Info::MaxNickBytes + 1; i++)
			{
				reader >> str[i];

				if (reader.tell() >= msg->length) // no more bytes?
					return false;

				if (i == 0 && str[i] == 0) // nickname with zero length
					return false;

				if (str[i] == 0) // end of string
					break;
			}

			// TODO: check if str is valid UTF-8

			if (reader.tell() + Player::SoldierState::Size > msg->length) // must fit
				return false;

			reader.seek(Player::SoldierState::Size, DataStream::Current);
		}

		return true;
	}

	void StartMessage::serialize(Message *msg)
	{
		DataWriter writer(data_, sizeof(data_));

		writer << (uint8_t)Message::Start;

		for (int i = 0; i < nPlayers; i++)
		{
			Player::Info *info = &playersInfo[i];

			uint8_t idAndState = (info->state & 0xF) | ((info->id << 4) & 0xF0);

			info->nickname[sizeof(info->nickname) - 1] = 0;

			writer << idAndState;
			writer << info->nickname;
			writer << info->soldierState;
		}

		msg->data = data_;
		msg->length = writer.tell();

		assert(validate(msg));
	}

	void StartMessage::unserialize(const Message *msg)
	{
		assert(validate(msg));

		DataReader reader(msg->data, msg->length);
		reader.seek(1, DataStream::Begin);

		nPlayers = 0;

		while (reader.tell() < msg->length)
		{
			Player::Info *info = &playersInfo[nPlayers++];

			uint8_t idAndState;

			reader >> idAndState;
			reader >> info->nickname;
			reader >> info->soldierState;

			info->id = idAndState >> 4;
			info->state = (Player::State)(idAndState & 0xF);
		}
	}
}
