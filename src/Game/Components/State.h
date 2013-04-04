#pragma once

namespace component
{

class State
{
public:
	vec2 position;
	vec2 velocity;
	vec2 acceleration;

	State operator - ();
	State operator + (const State &operand);
	State &operator += (const State &operand);
	State operator - (const State &operand);
	State &operator -= (const State &operand);
	State operator * (float operand);
	State &operator *= (float operand);
	State operator / (float operand);
	State &operator /= (float operand);
};

}
