#pragma once

#include <glm/glm.hpp>

namespace math
{
	template<typename T> class interpolable
	{
	public:
		T previous;
		T current;

		interpolable() : previous(), current() {}
		interpolable(T value) : previous(value), current(value) {}

		void update()     { previous = current; }
		void set(T value) { previous = current = value; }

		T value(float percent) const { return glm::mix(previous, current, percent); };
	};
}
