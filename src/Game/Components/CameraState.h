#pragma once

#include <Game/Components/State.h>

namespace component
{

class CameraState : public State
{
public:
	CameraState();

	float zoom;

	CameraState operator - ();
	CameraState operator + (const CameraState &operand);
	CameraState &operator += (const CameraState &operand);
	CameraState operator - (const CameraState &operand);
	CameraState &operator -= (const CameraState &operand);
	CameraState operator * (float operand);
	CameraState &operator *= (float operand);
	CameraState operator / (float operand);
	CameraState &operator /= (float operand);
};

}
