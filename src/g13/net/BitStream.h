#pragma once

#include <stdint.h>
#include <stddef.h>
#include <assert.h>

namespace g13 {
namespace net {

// this class assumes that signed integers are represented the same way in every system

class BitStream
{
public:
	BitStream(size_t nBytes) : bit_(0), size_(8 * nBytes) {}

	size_t bitpos()          const { return bit_; }
	bool   has (size_t bits) const { return bit_ + bits <= size_; }
	void   skip(size_t bits)       { bit_ += bits; }

protected:
	size_t bit_;
	size_t size_;
};

class BitReader : public BitStream
{
public:
	BitReader(const uint8_t *data, size_t nBytes) : BitStream(nBytes), data_(data) {}

	void read(int8_t  *x, size_t bits = sizeof(int8_t ) * 8) { read_ <uint8_t > ((uint8_t *)x, bits); }
	void read(int16_t *x, size_t bits = sizeof(int16_t) * 8) { read_ <uint16_t> ((uint16_t*)x, bits); }
	void read(int32_t *x, size_t bits = sizeof(int32_t) * 8) { read_ <uint32_t> ((uint32_t*)x, bits); }
	void read(int64_t *x, size_t bits = sizeof(int64_t) * 8) { read_ <uint64_t> ((uint64_t*)x, bits); }

	void read(uint8_t  *x, size_t bits = sizeof(uint8_t ) * 8) { read_ <uint8_t > (x, bits); }
	void read(uint16_t *x, size_t bits = sizeof(uint16_t) * 8) { read_ <uint16_t> (x, bits); }
	void read(uint32_t *x, size_t bits = sizeof(uint32_t) * 8) { read_ <uint32_t> (x, bits); }
	void read(uint64_t *x, size_t bits = sizeof(uint64_t) * 8) { read_ <uint64_t> (x, bits); }

	void read(bool *x)
	{
		uint8_t value;
		read_<uint8_t>(&value, 1);
		*x = (bool)value;
	}

	// returns string size excluding null
	// does not guarantee null termination (will return max in this case)
	size_t read(char *str, size_t max)
	{
		size_t i = 0;
		uint8_t ch = 1;

		while (i < max && ch != 0 && has(8))
		{
			read_<uint8_t>(&ch);
			str[i] = *(char*)&ch;
			i++;
		}

		return i;
	}

private:
	const uint8_t *data_;

	template<typename T> void read_(T *value, size_t bits = sizeof(T) * 8)
	{
		assert(has(bits) && bits <= sizeof(T) * 8);

		*value = 0;

		for (size_t i = 0; i < bits; i++)
		{
			*value <<= 1;
			*value |= (data_[(bit_ + i) / 8] >> (7 - ((bit_ + i) % 8))) & T(1);
		}

		bit_ += bits;
	}
};

class BitWriter : public BitStream
{
public:
	BitWriter(uint8_t *data, size_t nBytes) : BitStream(nBytes), data_(data) {}

	void write(int8_t  x, size_t bits = sizeof(int8_t ) * 8) { write_ <uint8_t > (*(uint8_t *)&x, bits); }
	void write(int16_t x, size_t bits = sizeof(int16_t) * 8) { write_ <uint16_t> (*(uint16_t*)&x, bits); }
	void write(int32_t x, size_t bits = sizeof(int32_t) * 8) { write_ <uint32_t> (*(uint32_t*)&x, bits); }
	void write(int64_t x, size_t bits = sizeof(int64_t) * 8) { write_ <uint64_t> (*(uint64_t*)&x, bits); }

	void write(uint8_t  x, size_t bits = sizeof(uint8_t ) * 8) { write_ <uint8_t > (x, bits); }
	void write(uint16_t x, size_t bits = sizeof(uint16_t) * 8) { write_ <uint16_t> (x, bits); }
	void write(uint32_t x, size_t bits = sizeof(uint32_t) * 8) { write_ <uint32_t> (x, bits); }
	void write(uint64_t x, size_t bits = sizeof(uint64_t) * 8) { write_ <uint64_t> (x, bits); }

	void write(bool x) { write_<uint8_t>((uint8_t)x, 1); }

	void write(const char *str, size_t max)
	{
		size_t i = 0;

		while (i < max && str[i] != 0 && has(8))
			write_<uint8_t>(*(uint8_t*)&str[i++]);

		write_<uint8_t>(0);
	}

private:
	uint8_t *data_;

	template<typename T> void write_(T value, size_t bits = sizeof(T) * 8)
	{
		assert(has(bits));

		for (size_t i = 0; i < bits; i++)
		{
			if ((value >> (bits - i - 1)) & T(1))
				data_[(bit_ + i) / 8] |= uint8_t(1) << (7 - ((bit_ + i) % 8));
			else
				data_[(bit_ + i) / 8] &= ~(uint8_t(1) << (7 - ((bit_ + i) % 8)));
		}

		bit_ += bits;
	}
};

}} // g13::net
