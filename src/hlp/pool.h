#pragma once

#include <stddef.h>
#include <vector>

namespace hlp {

template<typename T, size_t BlockSize = 32> class pool
{
public:
	pool() : front_(0), blocks_(1)
	{
		front_ = &initialBlock_.items[0];
		blocks_[0] = &initialBlock_;
	}

	~pool()
	{
		for (size_t i = 1; i < blocks_.size(); i++)
			delete blocks_[i];
	}

	T *alloc()
	{
		pool_item *item = front_;

		if (!item)
		{
			pool_block *block = new pool_block();
			item = &block->items[0];
			blocks_.push_back(block);
		}

		front_ = item->next;
		return &item->data;
	}

	void free(T *data)
	{
		static const size_t offset = (char*)&initialBlock_.items[0].data - (char*)&initialBlock_.items[0];

		pool_item *item = (pool_item*)((char*)data - offset);

		item->next = front_;
		front_ = item;
	}

private:
	struct pool_item
	{
		pool_item() : next(0) {}
		pool_item *next;
		T data;
	};

	struct pool_block
	{
		pool_block()
		{
			for (size_t i = 0; i < BlockSize - 1; i++)
				items[i].next = &items[i + 1];
		}

		pool_item items[BlockSize];
	};

	pool_item *front_;
	std::vector<pool_block*> blocks_;
	pool_block initialBlock_;
};

} // hlp
