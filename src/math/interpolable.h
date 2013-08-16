#pragma once

#include <glm/glm.hpp>

namespace math {

template<typename T> class interpolable
{
public:
	T previous;
	T current;

	interpolable()
		:	previous(),
			current(),
			interpolatedValue_()
	{}

	interpolable(T value)
		:	previous(value),
			current(value),
			interpolatedValue_(value)
	{}

	void set(T value)
	{
		previous = current = interpolatedValue_ = value;
	}

	T interpolate(float percent) const
	{
		return interpolatedValue_ = glm::mix(previous, current, percent);
	};

	operator const T&() const
	{
		return interpolatedValue_;
	}

private:
	mutable T interpolatedValue_;
};

} // math
