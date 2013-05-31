#pragma once

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <string>

namespace net
{
	class DataStream
	{
	public:
		DataStream(size_t size) : index_(0), size_(size) {}

		enum SeekMode { Begin, Current, End };

		size_t tell() const { return index_; }

		void seek(size_t position, SeekMode mode = Current)
		{
			switch (mode)
			{
				case Begin:   index_ = position;         break;
				case Current: index_ += position;        break;
				case End:     index_ = size_ - position; break;
			}
		}

	protected:
		size_t index_;
		size_t size_;
	};

	class DataReader : public DataStream
	{
	public:
		DataReader(const uint8_t *data, size_t size) : DataStream(size), data_(data) {}

		DataReader& operator>>(int8_t   &x) { read((uint8_t *)&x); return *this; }
		DataReader& operator>>(int16_t  &x) { read((uint16_t*)&x); return *this; }
		DataReader& operator>>(int32_t  &x) { read((uint32_t*)&x); return *this; }
		DataReader& operator>>(int64_t  &x) { read((uint64_t*)&x); return *this; }

		DataReader& operator>>(uint8_t  &x) { read(&x); return *this; }
		DataReader& operator>>(uint16_t &x) { read(&x); return *this; }
		DataReader& operator>>(uint32_t &x) { read(&x); return *this; }
		DataReader& operator>>(uint64_t &x) { read(&x); return *this; }

		DataReader& operator>>(std::string &str)
		{
			str.empty();

			while (index_ < size_ && data_[index_] != 0)
				str += *(char*)&data_[index_++];

			index_++;

			return *this;
		}

		DataReader& operator>>(char *str)
		{
			int i = 0;

			while (index_ < size_ && data_[index_] != 0)
			{
				str[i] = *(char*)&data_[index_++];
				++i;
			}

			str[i] = 0;
			index_++;

			return *this;
		}

	private:
		const uint8_t *data_;

		template<typename T> void read(T *value)
		{
			assert(index_ + sizeof(T) <= size_);

			*value = 0;

			for (size_t i = 0; i < sizeof(T); i++)
				*value |= T(data_[index_ + i]) << (8 * sizeof(T) - 8 * (i + 1));

			index_ += sizeof(T);
		}
	};

	class DataWriter : public DataStream
	{
	public:
		DataWriter(uint8_t *data, size_t size) : DataStream(size), data_(data) {}

		DataWriter& operator<<(int8_t   x) { write(*(uint8_t *)&x); return *this; }
		DataWriter& operator<<(int16_t  x) { write(*(uint16_t*)&x); return *this; }
		DataWriter& operator<<(int32_t  x) { write(*(uint32_t*)&x); return *this; }
		DataWriter& operator<<(int64_t  x) { write(*(uint64_t*)&x); return *this; }

		DataWriter& operator<<(uint8_t  x) { write(x); return *this; }
		DataWriter& operator<<(uint16_t x) { write(x); return *this; }
		DataWriter& operator<<(uint32_t x) { write(x); return *this; }
		DataWriter& operator<<(uint64_t x) { write(x); return *this; }

		DataWriter& operator<<(const std::string &str) { return *this << str.c_str(); }

		DataWriter& operator<<(const char *str)
		{
			int i = 0;

			while (index_ < size_ && str[i] != 0)
				data_[index_++] = *(uint8_t*)&str[i++];

			if (index_ < size_)
				data_[index_++] = 0;

			return *this;
		}

	private:
		uint8_t *data_;

		template<typename T> void write(T value)
		{
			assert(index_ + sizeof(T) <= size_);

			for (size_t i = 0; i < sizeof(T); i++)
				data_[index_ + i] = 0xFF & (value >> (8 * sizeof(T) - 8 * (i + 1)));

			index_ += sizeof(T);
		}
	};
}
