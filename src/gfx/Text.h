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

	void value(const std::basic_string<uint32_t> &str);
	void value(const char *str);
	void font(Font *font);
	void size(uint32_t size);
	void color(const Color &color);

	const std::basic_string<uint32_t> &value();
	Font *font() const;
	Color color() const;
	uint32_t size() const;

	const Bounds &bounds();

private:
	std::basic_string<uint32_t> value_;
	Font *font_;
	uint32_t size_;
	Color color_;

	Bounds bounds_;

	bool updatedGeometry_;
	bool updatedBounds_;

	struct DrawData
	{
		DrawData() : vbo(0), count(0), atlas(0), offset(0) {}
		DrawData(VBO *v, int c, int a) : vbo(v), count(c), atlas(a), offset(0) {}
		VBO *vbo;
		int count;
		int atlas;
		int offset;
	};

	IBO *ibo_;
	std::vector<VBO*> vbos_;
	std::vector<DrawData> drawData_;
	std::vector<const Font::Glyph*> glyphs_;

	void update(bool upload);
	void draw();

	friend void draw(Text *text);
};

} // gfx
