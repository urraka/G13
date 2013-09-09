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

	void update(Time dt);
	void frame(const Frame &frame);

	void target(const vec2 *target);
	void viewport(int width, int height);
	void zoom(ZoomType zoomType);

	const mat2d &matrix()    const { return matrix_; }
	const mat2d &matrixinv() const { return matrixinv_; }

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

	mat2d matrix_;
	mat2d matrixinv_;
};

}} // g13::ent
