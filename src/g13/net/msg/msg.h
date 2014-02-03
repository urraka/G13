#pragma once

#include <g13/net/constants.h>
#include <g13/cmp/SoldierState.h>
#include <g13/cmp/BulletParams.h>
#include <g13/math.h>
#include <assert.h>
#include <stdint.h>

namespace g13
{
	namespace net
	{
		namespace msg
		{
			class Message
			{
			public:
				Message(int type_, int channel_) : type(type_), channel(channel_) {}

				virtual size_t serialize(uint8_t *data, size_t length) const = 0;

				const int type;
				const int channel;
			};

			class Storage
			{
			public:
				uint8_t data[1400 - 28];
			};

			Message *create(const uint8_t *data, size_t length);
			void destroy(Message *msg);
		}
	}
}

#define MSGMAGIC_DECLARE
#include "msg_magic.h"
