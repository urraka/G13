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
	void bounds(const vec2 &tl, const vec2 &br);

	const mat2d &matrix()    const { return matrix_; }
	const mat2d &matrixinv() const { return matrixinv_; }
	const vec2  viewport()   const { return vec2(width_, height_); }

	float defaultScale() const;

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
	vec2 tlBounds_;
	vec2 brBounds_;

	void clampToBounds();
};

}} // g13::ent
