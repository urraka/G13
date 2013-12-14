#pragma once

#include "Font.h"
#include "Color.h"

#include <string>
#include <vector>
#include <stdint.h>

namespace gfx {

class VBO;
class IBO;

class Text
{
public:
	Text();
	~Text();

	struct Bounds
	{
		Bounds() : x(0), y(0), width(0), height(0) {}
		float x;
		float y;
		float width;
		float height;
	};

	typedef std::basic_string<uint32_t> string32_t;

	void value(const string32_t &str);
	void value(const char *str);
	void font(Font *font);
	void size(uint32_t size);
	void color(const Color &color);

	void outlineColor(const Color &color);
	void outlineWidth(float width);

	const string32_t &value();
	Font *font() const;
	Color color() const;
	uint32_t size() const;

	const Bounds &bounds();

	struct DrawData
	{
		DrawData() : vbo(0), count(0), atlas(0), offset(0) {}
		DrawData(VBO *v, int c, int a) : vbo(v), count(c), atlas(a), offset(0) {}
		VBO *vbo;
		int count;
		int atlas;
		int offset;
	};

private:
	string32_t value_;
	Font *font_;
	uint32_t size_;
	Color color_;
	float outlineWidth_;
	Color outlineColor_;

	Bounds bounds_;

	bool updatedGeometry_;
	bool updatedBounds_;

	IBO *ibo_;
	std::vector<VBO*> vbos_;
	std::vector<DrawData> drawData_;
	std::vector<const Font::Glyph*> glyphs_;

	std::vector<DrawData> outlineDrawData_;
	std::vector<const Font::Glyph*> outlineGlyphs_;

	void update(bool upload);
	void draw();

	friend void draw(Text *text);
};

} // gfx
