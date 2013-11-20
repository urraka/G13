#pragma once

#include <g13/g13.h>
#include <g13/math.h>
#include <gfx/forward.h>
#include <gfx/Color.h>

namespace g13 {
namespace ui {

class HealthBar
{
public:
	HealthBar();
	~HealthBar();

	void setSize(float width, float height);
	void setPosition(float x, float y);
	void setHealth(float percent);
	void setOutline(float width, const gfx::Color &color);
	void setFill(const gfx::Color &color);
	void setOpacity(float opacity);

	void draw();

private:
	bool changed_;
	float health_;
	vec2 position_;
	vec2 size_;

	float opacity_;
	float outlineWidth_;

	gfx::Color outlineColor_;
	gfx::Color fillColor_;

	gfx::VBO *vbo_;
};

}} // g13::ui
