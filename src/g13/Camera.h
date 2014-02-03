#pragma once

#include <g13/g13.h>
#include <g13/math.h>

namespace g13 {

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

	void frame(const Frame &frame);

	void setPosition(const vec2 &position);
	void target(const vec2 &target);
	void viewport(int width, int height);
	void zoom(ZoomType zoomType);
	void bounds(const vec2 &tl, const vec2 &br);

	vec2  viewport()       const { return vec2(width_, height_); }
	float viewportWidth()  const { return width_;  }
	float viewportHeight() const { return height_; }

	const mat2d &matrix()    const { return matrix_; }
	const mat2d &matrixinv() const { return matrixinv_; }

	float defaultScale() const;

private:
	float zoom_;
	float zoomTarget_;
	ZoomType zoomType_;

	float width_;
	float height_;
	vec2 tlBounds_;
	vec2 brBounds_;

	vec2 target_;
	vec2 position_;

	mat2d matrix_;
	mat2d matrixinv_;

	void clampToBounds();
};

} // g13
