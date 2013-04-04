#pragma once

#include <Game/Components/CameraState.h>

namespace component
{

class Camera
{
public:
	Camera();

	void update(Time dt, CameraState &state);
	void target(const vec2 *target);
	void viewport(int width, int height);
	mat4 matrix(const CameraState &state);

private:
	const vec2 *target_;
	ivec2 viewport_;
};

}
