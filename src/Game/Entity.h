#pragma once

class Entity
{
public:
	virtual void update(Time dt) = 0;
	const vec2 &position() const;

protected:
	enum
	{
		PreviousFrame = 0,
		CurrentFrame = 1
	};

	vec2 position_[2];
};
