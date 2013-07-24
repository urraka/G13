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

	void value(const std::basic_string<uint32_t> &str);
	void value(const char *str);
	void font(Font *font);
	void size(uint32_t size);
	void color(const Color &color);

private:
	std::basic_string<uint32_t> value_;
	Font *font_;
	uint32_t size_;
	Color color_;

	bool updated_;

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

	void update();
	void draw();

	friend void draw(Text *text);
};

} // gfx
