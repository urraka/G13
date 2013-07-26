#pragma once

#include <g13/g13.h>
#include <g13/math.h>

namespace g13 {
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

	void update(Time dt);
	void target(const vec2 *target);
	void viewport(int width, int height);
	float scale(float framePercent) const;
	mat4 matrix(float framePercent, MatrixMode mode = MatrixNormal) const;
	void zoom(ZoomType zoomType);

private:
	math::interpolable<float> zoom_;
	float maxZoom_;
	float zoomRate_;
	float zoomVelocity_;
	float zoomTarget_;
	ZoomType zoomType_;
	float width_;
	float height_;
	const vec2 *target_;
	math::interpolable<vec2> position_;
	vec2 velocity_;
};

}} // g13::ent
