#include "enet_callbacks.h"
#include <hlp/pool.h>
#include <iostream>
#include <assert.h>
#include <stdlib.h>

#define WRAPPER_OFFSET(T)    (offsetof(struct wrapper_t<T>, data))
#define WRAPPER_PTR(data, T) (wrapper_t<T>*)((char*)data - WRAPPER_OFFSET(T))
#define MALLOC_OFFSET        (offsetof(struct {char a; ::hlp::structures_t b;}, b))

namespace hlp {

static void *cb_malloc(size_t size);
static void  cb_free(void *data);

template<typename T> struct wrapper_t
{
	char type; // not directly used, but at least 1 byte is needed here
	T data;
};

enum
{
	Packet = 0,
	OutgoingCommand,
	Acknowledgement,
	Other
};

struct callbacks_t
{
	callbacks_t() : enet(), pckCount(0), outCount(0), ackCount(0), otherCount(0)
	{
		enet.malloc = cb_malloc;
		enet.free = cb_free;
	}

	ENetCallbacks enet;

	hlp::pool<wrapper_t<ENetPacket>,          128> pckPool;
	hlp::pool<wrapper_t<ENetOutgoingCommand>, 128> outPool;
	hlp::pool<wrapper_t<ENetAcknowledgement>, 128> ackPool;

	int pckCount;
	int outCount;
	int ackCount;
	int otherCount;
};

// This is to calculate memory alignment.. just in case. ENetRangeCoder couldn't make it here,
// but it only has uint8 and uint16, so it should be covered.

struct structures_t
{
	ENetPacket          packet;
	ENetHost            host;
	ENetPeer            peer;
	ENetOutgoingCommand outgoingCommand;
	ENetAcknowledgement acknowledgement;
	ENetChannel         channel;
};

static callbacks_t callbacks;

const ENetCallbacks *enet_callbacks = &callbacks.enet;

void *cb_malloc(size_t size)
{
	void *result = 0;
	int type;

	switch (size)
	{
		case sizeof(ENetPacket):
		{
			type = Packet;
			wrapper_t<ENetPacket> *storage = callbacks.pckPool.alloc();
			result = &storage->data;

			callbacks.pckCount++;
		}
		break;

		case sizeof(ENetOutgoingCommand):
		{
			type = OutgoingCommand;
			wrapper_t<ENetOutgoingCommand> *storage = callbacks.outPool.alloc();
			result = &storage->data;

			callbacks.outCount++;
		}
		break;

		case sizeof(ENetAcknowledgement):
		{
			type = Acknowledgement;
			wrapper_t<ENetAcknowledgement> *storage = callbacks.ackPool.alloc();
			result = &storage->data;

			callbacks.ackCount++;
		}
		break;

		default:
		{
			type = Other;
			result = (char*)malloc(size + MALLOC_OFFSET) + MALLOC_OFFSET;

			callbacks.otherCount++;
		}
		break;
	}

	if (result != 0)
		((char*)result)[-1] = type;

	return result;
}

void cb_free(void *data)
{
	if (data == 0)
		return;

	int type = ((char*)data)[-1];

	switch (type)
	{
		case Packet:
			callbacks.pckPool.free(WRAPPER_PTR(data, ENetPacket));
			break;

		case OutgoingCommand:
			callbacks.outPool.free(WRAPPER_PTR(data, ENetOutgoingCommand));
			break;

		case Acknowledgement:
			callbacks.ackPool.free(WRAPPER_PTR(data, ENetAcknowledgement));
			break;

		case Other:
			free((char*)data - MALLOC_OFFSET);
			break;

		default:
			assert(false);
			break;
	}
}

void enet_callbacks_counters()
{
	std::cout << std::endl << "ENet allocation counters:" << std::endl;

	std::cout << "ENetPacket:          " << callbacks.pckCount   << std::endl;
	std::cout << "ENetOutgoingCommand: " << callbacks.outCount   << std::endl;
	std::cout << "ENetAcknowledgement: " << callbacks.ackCount   << std::endl;
	std::cout << "Other:               " << callbacks.otherCount << std::endl;
}

} // hlp
