#pragma once

#include "../../Math/math.h"
#include <sys/sys.h>

namespace ent {

class Camera
{
public:
	Camera();

	enum ZoomType
	{
		ZoomNone = 0,
		ZoomIn = 1,
		ZoomOut = -1
	};

	enum MatrixMode
	{
		MatrixNormal,
		MatrixInverted
	};

	void update(sys::Time dt);
	void target(const vec2 *target);
	void viewport(int width, int height);
	mat4 matrix(float framePercent, MatrixMode mode = MatrixNormal);
	void zoom(ZoomType zoomType);

private:
	math::interpolable<float> zoom_;
	float maxZoom_;
	float zoomRate_;
	float zoomVelocity_;
	float zoomTarget_;
	ZoomType zoomType_;
	ivec2 viewport_;
	const vec2 *target_;
	math::interpolable<vec2> position_;
	vec2 velocity_;
};

} // ent
