#pragma once

class Entity
{
public:
	const vec2 &position() const;

protected:
	enum
	{
		PreviousFrame = 0,
		CurrentFrame = 1
	};

	vec2 position_[2];
	vec2 velocity_;
};
