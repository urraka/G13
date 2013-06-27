#pragma once

#include <stdint.h>
#include <stddef.h>

class Replay;

namespace cmp
{
	class SoldierInput
	{
	public:
		SoldierInput();
		void update();
		void reset();
		uint8_t serialize() const;
		void unserialize(uint8_t data);

		bool left;
		bool right;
		bool jump;
		bool run;
		bool duck;

		static const size_t MaxBits = 5;
	};
}
