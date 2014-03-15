#include "enet_callbacks.h"
#include <iostream>
#include <stdlib.h>

class DynamicPool
{
	struct Header { void *next; };
	static const size_t Padding = sizeof(Header);

public:
	DynamicPool(size_t dataSize, size_t blockSize)
		:	dataSize_(dataSize),
			blockSize_(blockSize),
			front_(0),
			blocks_(0)
	#ifdef DEBUG
			, allocatedBytes_(0),
			allocatedBlocks_(0)
	#endif
	{
		blocks_ = createBlock();
		front_ = (char*)blocks_ + Padding;
	}

	~DynamicPool()
	{
		while (blocks_ != 0)
		{
			Header *next = (Header*)((Header*)blocks_)->next;
			free(blocks_);
			blocks_ = next;
		}
	}

	void *alloc()
	{
		void *item = front_;

		if (item == 0)
		{
			Header *blockHeader = (Header*)createBlock();
			blockHeader->next = blocks_;
			blocks_ = blockHeader;

			item = (char*)blockHeader + Padding;
		}

		front_ = ((Header*)item)->next;

		return (char*)item + Padding;
	}

	void free(void *data)
	{
		Header *itemHeader = (Header*)((char*)data - Padding);
		itemHeader->next = front_;
		front_ = itemHeader;
	}

	#ifdef DEBUG
	size_t allocatedBytes() const { return allocatedBytes_; }
	size_t allocatedBlocks() const { return allocatedBlocks_; }
	#endif

private:
	const size_t dataSize_;
	const size_t blockSize_;
	void *front_;
	void *blocks_;

	#ifdef DEBUG
	size_t allocatedBytes_;
	size_t allocatedBlocks_;
	#endif

	void *createBlock()
	{
		const size_t itemSize = Padding + dataSize_;

		void *block = malloc(Padding + blockSize_ * itemSize);

		#ifdef DEBUG
		allocatedBlocks_++;
		allocatedBytes_ += Padding + blockSize_ * itemSize;
		#endif

		((Header*)block)->next = 0;

		Header *itemHeader = (Header*)((char*)block + Padding);

		for (size_t i = 0; i < blockSize_ - 1; i++)
		{
			itemHeader->next = (char*)itemHeader + itemSize;
			itemHeader = (Header*)itemHeader->next;
		}

		itemHeader->next = 0; // last one

		return block;
	}
};

#define DATA_SIZE(i)  (Base << i)
#define BLOCK_SIZE(i) (1 << (PoolCount - (i + 1)))

static const size_t Base = 32;
static const size_t Offset = sizeof(void*);
static const size_t PoolCount = 6;

struct PoolsWrapper
{
	DynamicPool *pools_[PoolCount];

	PoolsWrapper()
	{
		for (size_t i = 0; i < PoolCount; i++)
			pools_[i] = new DynamicPool(DATA_SIZE(i), BLOCK_SIZE(i));
	}

	~PoolsWrapper()
	{
		for (size_t i = 0; i < PoolCount; i++)
			delete pools_[i];
	}

	DynamicPool *operator[](int i) { return pools_[i]; }
};

static PoolsWrapper pools;

#ifdef DEBUG
static int counters[PoolCount + 1] = {};
#endif

static void *cb_malloc(size_t size)
{
	size_t i = 0;
	size_t n = Base;
	size_t expandedSize = size + Offset;

	while (n < expandedSize)
	{
		i++;
		n = n << 1;
	}

	if (i < PoolCount)
	{
		#ifdef DEBUG
		counters[i]++;
		#endif

		char *data = (char*)pools[i]->alloc();
		data[0] = i;
		return data + Offset;
	}
	else
	{
		#ifdef DEBUG
		counters[PoolCount]++;
		#endif

		char *data = (char*)malloc(size + Offset);
		data[0] = -1;
		return data + Offset;
	}
}

static void cb_free(void *data)
{
	char *buffer = (char*)data - Offset;

	if (buffer[0] == -1)
		free(buffer);
	else
		pools[(int)buffer[0]]->free(buffer);
}

namespace hlp
{
	const ENetCallbacks enet_callbacks = {cb_malloc, cb_free, 0};

	#ifdef DEBUG
	void enet_callbacks_counters()
	{
		size_t mallocCount = 0;
		size_t totalMemory = 0;

		std::cout << std::endl << "ENet allocation info:" << std::endl;

		for (size_t i = 0; i < PoolCount; i++)
		{
			mallocCount += pools[i]->allocatedBlocks();
			totalMemory += pools[i]->allocatedBytes();

			std::cout << "Pool #" << i << " (" << DATA_SIZE(i) << " bytes): " << std::endl <<
				"    Allocation count: " << counters[i] << std::endl <<
				"    Allocated bytes: " << pools[i]->allocatedBytes() << std::endl <<
				"    Allocated blocks: " << pools[i]->allocatedBlocks() << std::endl;
		}

		std::cout << "Total malloc calls: " << mallocCount << std::endl;
		std::cout << "Total memory allocated: " << totalMemory << std::endl;
		std::cout << "Non-pooled allocations: " << counters[PoolCount] << std::endl;
	}
	#endif
}
