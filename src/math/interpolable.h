#pragma once

#include <glm/glm.hpp>

namespace math {

namespace _ {
	template<typename T> inline T mix_default(T const &a, T const &b, float const &percent)
	{
		return glm::mix(a, b, percent);
	}

	template<typename T> struct mix_t
	{
		typedef T (*type)(T const&, T const&, float const&);
	};
}

template<typename T, typename _::mix_t<T>::type F = _::mix_default<T> > class interpolable
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

	const T &get() const
	{
		return interpolatedValue_;
	}

	const T &interpolate(float percent)
	{
		return interpolatedValue_ = F(previous, current, percent);
	};

	operator const T&() const
	{
		return interpolatedValue_;
	}

private:
	T interpolatedValue_;
};

} // math
