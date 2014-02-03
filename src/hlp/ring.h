#pragma once

#include <stddef.h>
#include <assert.h>

namespace hlp {

template<typename T, size_t N> class ring
{
public:
	ring() : head_(0), size_(0) {}

	void push(const T &element)
	{
		if (size_ == N)
		{
			buffer_[head_++] = element;
			head_ %= N;
		}
		else
		{
			buffer_[(head_ + size_) % N] = element;
			size_++;
		}
	}

	void clear()
	{
		head_ = 0;
		size_ = 0;
	}

	size_t size() const
	{
		return size_;
	}

	size_t capacity() const
	{
		return N;
	}

	bool full() const
	{
		return size_ == N;
	}

	const T &operator[](size_t index) const
	{
		assert(index < size_);
		return buffer_[(head_ + index) % N];
	}

	T &operator[](size_t index)
	{
		assert(index < size_);
		return buffer_[(head_ + index) % N];
	}

	const T &front() const { return (*this)[0]; }
	const T &back() const { return (*this)[size_ - 1]; }

	T &front() { return (*this)[0]; }
	T &back() { return (*this)[size_ - 1]; }

private:
	T buffer_[N];
	size_t head_;
	size_t size_;
};

} // hlp
