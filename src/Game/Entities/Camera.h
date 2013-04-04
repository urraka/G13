#pragma once

class Camera : public Entity
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
	void target(const Entity *target);
	void viewport(int width, int height);
	mat4 matrix(float framePercent);
	void zoom(ZoomType zoomType);

private:
	float zoom_[2];
	float maxZoom_;
	float zoomRate_;
	float zoomVelocity_;
	float zoomTarget_;
	ZoomType zoomType_;
	ivec2 viewport_;
	const Entity *target_;
};
