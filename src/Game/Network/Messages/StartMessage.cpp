#include "Message.h"
#include "StartMessage.h"
#include "../DataStream.h"
#include <cstring>
#include <assert.h>

namespace net
{
	bool StartMessage::validate(const Message *msg)
	{
		assert(false);
		return false;
	}

	void StartMessage::serialize(Message *msg)
	{
		DataWriter writer(data_, sizeof(data_));

		writer << (uint8_t)Message::Start;

		for (int i = 0; i < playersCount; i++)
		{
			Player::Info *info = &playersInfo[i];

			uint8_t idAndState = (info->state & 0xF) | ((info->id << 4) & 0xF0);
			uint8_t spriteState = (info->flipped ? (1 << 7) : 0) | (info->frame & 0x7F);

			info->nickname[sizeof(info->nickname) - 1] = 0;

			writer << idAndState;
			writer << info->nickname;
			writer << info->position.x.value();
			writer << info->position.y.value();
			writer << info->velocity.x.value();
			writer << info->velocity.y.value();
			writer << spriteState;
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

		playersCount = 0;

		while (reader.tell() < msg->length)
		{
			Player::Info *info = &playersInfo[playersCount++];

			uint8_t idAndState, spriteState;
			int32_t px, py, vx, vy;

			reader >> idAndState;
			reader >> info->nickname;
			reader >> px;
			reader >> py;
			reader >> vx;
			reader >> vy;
			reader >> spriteState;

			info->id = idAndState >> 4;
			info->state = (Player::State)(idAndState & 0xF);
			info->position.x = fixed::from_value(px);
			info->position.y = fixed::from_value(py);
			info->velocity.x = fixed::from_value(vx);
			info->velocity.y = fixed::from_value(vy);
			info->flipped = spriteState & 0x80;
			info->frame = spriteState & 0x7F;
		}
	}
}
