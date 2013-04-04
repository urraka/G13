#pragma once

#include <Game/Components/Camera.h>

class Camera
{
public:
	void update(Time dt);
	void target(const vec2 *target);
	void viewport(int width, int height);
	mat4 matrix(float percent);

private:
	component::Camera camera_;
	component::CameraState prevState_;
	component::CameraState state_;
};
