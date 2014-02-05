#include "enet_callbacks.h"
#include <hlp/pool.h>
#include <hlp/countof.h>
#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

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

template<size_t size> struct static_buffer { uint64_t data[size / sizeof(uint64_t)]; };

typedef static_buffer<32>   static_buffer32;
typedef static_buffer<64>   static_buffer64;
typedef static_buffer<128>  static_buffer128;
typedef static_buffer<256>  static_buffer256;
typedef static_buffer<512>  static_buffer512;
typedef static_buffer<1024> static_buffer1024;
typedef static_buffer<2048> static_buffer2048;

enum
{
	Packet = 0,
	OutgoingCommand,
	IncomingCommand,
	Acknowledgement,
	StaticBuffer32,
	StaticBuffer64,
	StaticBuffer128,
	StaticBuffer256,
	StaticBuffer512,
	StaticBuffer1024,
	StaticBuffer2048,
	Other
};

struct callbacks_t
{
	callbacks_t()
		:	enet(),
			pckCount(0),
			outCount(0),
			incCount(0),
			ackCount(0),
			bufsCount(),
			otherCount(0),
			otherCounts()
	{
		enet.malloc = cb_malloc;
		enet.free = cb_free;
	}

	ENetCallbacks enet;

	hlp::pool<wrapper_t<ENetPacket>,          32> pckPool;
	hlp::pool<wrapper_t<ENetOutgoingCommand>, 32> outPool;
	hlp::pool<wrapper_t<ENetIncomingCommand>, 32> incPool;
	hlp::pool<wrapper_t<ENetAcknowledgement>, 32> ackPool;
	hlp::pool<wrapper_t<static_buffer32>,     64> buf32Pool;
	hlp::pool<wrapper_t<static_buffer64>,     32> buf64Pool;
	hlp::pool<wrapper_t<static_buffer128>,    16> buf128Pool;
	hlp::pool<wrapper_t<static_buffer256>,     8> buf256Pool;
	hlp::pool<wrapper_t<static_buffer512>,     4> buf512Pool;
	hlp::pool<wrapper_t<static_buffer1024>,    2> buf1024Pool;
	hlp::pool<wrapper_t<static_buffer2048>,    1> buf2048Pool;

	int pckCount;
	int outCount;
	int incCount;
	int ackCount;
	int bufsCount[7];
	int otherCount;

	int otherCounts[32];
};

// This is to calculate memory alignment.. just in case. ENetRangeCoder couldn't make it here,
// but it only has uint8 and uint16, so it should be covered.

struct structures_t
{
	ENetPacket          packet;
	ENetHost            host;
	ENetPeer            peer;
	ENetOutgoingCommand outgoingCommand;
	ENetIncomingCommand incomingCommand;
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

		case sizeof(ENetIncomingCommand):
		{
			type = IncomingCommand;
			wrapper_t<ENetIncomingCommand> *storage = callbacks.incPool.alloc();
			result = &storage->data;

			callbacks.incCount++;
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
			assert(size > 0);

			if (size <= sizeof(static_buffer32))
			{
				type = StaticBuffer32;
				wrapper_t<static_buffer32> *storage = callbacks.buf32Pool.alloc();
				result = &storage->data;
				callbacks.bufsCount[0]++;
			}
			else if (size <= sizeof(static_buffer64))
			{
				type = StaticBuffer64;
				wrapper_t<static_buffer64> *storage = callbacks.buf64Pool.alloc();
				result = &storage->data;
				callbacks.bufsCount[1]++;
			}
			else if (size <= sizeof(static_buffer128))
			{
				type = StaticBuffer128;
				wrapper_t<static_buffer128> *storage = callbacks.buf128Pool.alloc();
				result = &storage->data;
				callbacks.bufsCount[2]++;
			}
			else if (size <= sizeof(static_buffer256))
			{
				type = StaticBuffer256;
				wrapper_t<static_buffer256> *storage = callbacks.buf256Pool.alloc();
				result = &storage->data;
				callbacks.bufsCount[3]++;
			}
			else if (size <= sizeof(static_buffer512))
			{
				type = StaticBuffer512;
				wrapper_t<static_buffer512> *storage = callbacks.buf512Pool.alloc();
				result = &storage->data;
				callbacks.bufsCount[4]++;
			}
			else if (size <= sizeof(static_buffer1024))
			{
				type = StaticBuffer1024;
				wrapper_t<static_buffer1024> *storage = callbacks.buf1024Pool.alloc();
				result = &storage->data;
				callbacks.bufsCount[5]++;
			}
			else if (size <= sizeof(static_buffer2048))
			{
				type = StaticBuffer2048;
				wrapper_t<static_buffer2048> *storage = callbacks.buf2048Pool.alloc();
				result = &storage->data;
				callbacks.bufsCount[6]++;
			}
			else
			{
				type = Other;
				result = (char*)malloc(size + MALLOC_OFFSET) + MALLOC_OFFSET;

				size_t i = 0;
				while ((1u << i) < size) i++;
				assert(i < countof(callbacks.otherCounts));

				callbacks.otherCounts[i]++;
				callbacks.otherCount++;
			}
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

		case IncomingCommand:
			callbacks.incPool.free(WRAPPER_PTR(data, ENetIncomingCommand));
			break;

		case Acknowledgement:
			callbacks.ackPool.free(WRAPPER_PTR(data, ENetAcknowledgement));
			break;

		case StaticBuffer32:
			callbacks.buf32Pool.free(WRAPPER_PTR(data, static_buffer32));
			break;

		case StaticBuffer64:
			callbacks.buf64Pool.free(WRAPPER_PTR(data, static_buffer64));
			break;

		case StaticBuffer128:
			callbacks.buf128Pool.free(WRAPPER_PTR(data, static_buffer128));
			break;

		case StaticBuffer256:
			callbacks.buf256Pool.free(WRAPPER_PTR(data, static_buffer256));
			break;

		case StaticBuffer512:
			callbacks.buf512Pool.free(WRAPPER_PTR(data, static_buffer512));
			break;

		case StaticBuffer1024:
			callbacks.buf1024Pool.free(WRAPPER_PTR(data, static_buffer1024));
			break;

		case StaticBuffer2048:
			callbacks.buf2048Pool.free(WRAPPER_PTR(data, static_buffer2048));
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

	std::cout << "ENetPacket:          " << callbacks.pckCount     << std::endl;
	std::cout << "ENetOutgoingCommand: " << callbacks.outCount     << std::endl;
	std::cout << "ENetIncomingCommand: " << callbacks.incCount     << std::endl;
	std::cout << "ENetAcknowledgement: " << callbacks.ackCount     << std::endl;
	std::cout << "static_buffer32:     " << callbacks.bufsCount[0] << std::endl;
	std::cout << "static_buffer64:     " << callbacks.bufsCount[1] << std::endl;
	std::cout << "static_buffer128:    " << callbacks.bufsCount[2] << std::endl;
	std::cout << "static_buffer256:    " << callbacks.bufsCount[3] << std::endl;
	std::cout << "static_buffer512:    " << callbacks.bufsCount[4] << std::endl;
	std::cout << "static_buffer1024:   " << callbacks.bufsCount[5] << std::endl;
	std::cout << "static_buffer2048:   " << callbacks.bufsCount[6] << std::endl;
	std::cout << "Other:               " << callbacks.otherCount   << std::endl;

	std::cout << std::endl;

	for (size_t i = 0; i < countof(callbacks.otherCounts); i++)
		std::cout << "otherCounts[" << i << "]: " << callbacks.otherCounts[i] << std::endl;
}

} // hlp
