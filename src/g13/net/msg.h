#pragma once

#include "Player.h"
#include "Multiplayer.h"

#include <g13/math.h>

#include <assert.h>
#include <stdint.h>

namespace g13 {
namespace net {
namespace msg {

class Message
{
public:
	virtual uint8_t type() const = 0;
	virtual size_t serialize(uint8_t *data, size_t length) const = 0;
};

struct Storage
{
	uint8_t data[1400 - 28];
};

Message *create(const uint8_t *data, size_t length);
void destroy(Message *msg);

}}} // g13::net::msg

#define MSGMAGIC_DECLARE
#include "msg_magic.h"
