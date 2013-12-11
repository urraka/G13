#pragma once

#include <g13/g13.h>
#include <gfx/forward.h>

namespace g13 {
namespace ui {

class TextBox
{
public:
	TextBox(gfx::Font *font);

	void show();
	void hide();
	void clear();

	void setFont(gfx::Font *font);
	void setColor(const gfx::Color &color);
	void setBackgroundColor(const gfx::Color &color);
	void setPosition(float x, float y);
	void setWidth(float width);
	void setMultiline(bool multiline);

	gfx::Font *font();
	gfx::Color color() const;
	gfx::Color backgroundColor() const;

	float width() const;
	float height() const;

	const string32_t &value() const;

	void event(Event *evt);
	void draw();

private:
	string32_t  value_;
	vec2        position_;
	vec2        size_;
	vec2        padding_;
	vec2        caret_;
	bool        visible_;
	bool        multiline_;
	bool        changed_;
	gfx::Text  *text_;
	gfx::VBO   *background_;
	gfx::Color  color_;
	gfx::Color  backgroundColor_;
};

}} // g13::ui
