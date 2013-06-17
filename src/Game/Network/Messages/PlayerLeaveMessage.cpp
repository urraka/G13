#include "Message.h"
#include "PlayerLeaveMessage.h"
#include "../Server.h"
#include <assert.h>

namespace net
{
	bool PlayerLeaveMessage::validate(const Message *msg)
	{
		assert(msg->type() == Message::PlayerLeave);

		return msg->length == sizeof(PlayerLeaveMessage::data_) && msg->data[1] < Server::MaxPeers;
	}

	void PlayerLeaveMessage::serialize(Message *msg)
	{
		data_[0] = Message::PlayerLeave;
		data_[1] = id;

		msg->data = data_;
		msg->length = sizeof(data_);

		assert(validate(msg));
	}

	void PlayerLeaveMessage::unserialize(const Message *msg)
	{
		assert(validate(msg));

		id = msg->data[1];
	}
}
