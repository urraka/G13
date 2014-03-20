#include "enet_callbacks.h"
#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>
#include <assert.h>

class MemoryPool
{
public:
	struct Header
	{
		union {
			void *next;
			uint64_t x;
		};
	};

	static const size_t Padding = sizeof(Header);

	MemoryPool()
		:	dataSize_(0),
			blockSize_(0),
			front_(0),
			blocks_(0)
			#ifdef DEBUG
			, allocatedBytes_(0),
			allocatedBlocks_(0)
			#endif
	{
	}

	~MemoryPool()
	{
		while (blocks_ != 0)
		{
			Header *next = (Header*)((Header*)blocks_)->next;
			free(blocks_);
			blocks_ = next;
		}
	}

	void initialize(size_t dataSize, size_t blockSize)
	{
		assert(dataSize > 0 && blockSize > 0 && blocks_ == 0);

		dataSize_ = Padding * (size_t)ceil(dataSize / (float)Padding);
		blockSize_ = blockSize;
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
	size_t dataSize_;
	size_t blockSize_;
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

static const size_t BaseBit = 5;
static const size_t Base = 1 << BaseBit;
static const size_t Padding = MemoryPool::Padding;
static const size_t PoolCount = 6;

static MemoryPool *initialize_pools()
{
	static MemoryPool pools[PoolCount];

	for (size_t i = 0; i < PoolCount; i++)
		pools[i].initialize(DATA_SIZE(i), BLOCK_SIZE(i));

	return pools;
}

static MemoryPool *pools = initialize_pools();

#ifdef DEBUG
static int counters[PoolCount + 1] = {};
#endif

#ifdef _MSC_VER
#include <intrin.h>
// TODO: this should be verified... some day
static inline size_t clz(unsigned int x)     { unsigned long n; _BitScanReverse(&n, x); return 31 - n; }
static inline size_t clz(unsigned __int64 x) { unsigned __int64 n; _BitScanReverse64(&n, x); return 63 - n; }
#else
static inline size_t clz(unsigned int x)       { return __builtin_clz(x);   }
static inline size_t clz(unsigned long long x) { return __builtin_clzll(x); }
#endif

static void *cb_malloc(size_t size)
{
	size_t i = (sizeof(size_t) * CHAR_BIT) - clz(size_t((size + Padding - 1) | (Base - 1))) - BaseBit;

	if (i < PoolCount)
	{
		assert(DATA_SIZE(i) >= size + Padding);

		#ifdef DEBUG
		counters[i]++;
		#endif

		char *data = (char*)pools[i].alloc();
		data[0] = i;
		return data + Padding;
	}
	else
	{
		#ifdef DEBUG
		counters[PoolCount]++;
		#endif

		char *data = (char*)malloc(size + Padding);
		data[0] = -1;
		return data + Padding;
	}
}

static void cb_free(void *data)
{
	char *buffer = (char*)data - Padding;

	if (buffer[0] == -1)
		free(buffer);
	else
		pools[(int)buffer[0]].free(buffer);
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
			mallocCount += pools[i].allocatedBlocks();
			totalMemory += pools[i].allocatedBytes();

			std::cout << "Pool #" << i << " (" << DATA_SIZE(i) << " bytes): " << std::endl <<
				"    Allocation count: " << counters[i] << std::endl <<
				"    Allocated bytes: " << pools[i].allocatedBytes() << std::endl <<
				"    Allocated blocks: " << pools[i].allocatedBlocks() << std::endl;
		}

		std::cout << "Total malloc calls: " << mallocCount << std::endl;
		std::cout << "Total memory allocated: " << totalMemory << std::endl;
		std::cout << "Non-pooled allocations: " << counters[PoolCount] << std::endl;
	}
	#endif
}
